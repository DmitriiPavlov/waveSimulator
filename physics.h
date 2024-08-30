#include <vector>
#include <cassert>
#include <thread>
#define THREAD_RK4 false
class Point {
public:
//    Point(float y, float vy){
//        this->y = y;
//        this->vy = vy;
//    }
    float y, vy;
};

struct ThreadParams {
public:
    std::vector<float> * system;
    std::vector<float> * out;
    int startY;
    int endY;
};

class WaveLine{
private:
    std::vector<float> intermediate;
    std::vector<float> k1;
    std::vector<float> k2;
    std::vector<float> k3;
    std::vector<float> k4;
public:
    std::vector<float> y;
    float restoringK;
    float kvalue, bvalue;
    std::vector<Point> points;
    std::vector<Point> pointsdt;

    //we are going to assume the equilibrium is 0
    WaveLine(float restoringK,float kvalue, float bvalue, int length){

        points.resize(length,Point{0,0});
        pointsdt.resize(length,Point{0,0});
        //this is to contain the whole system for the purpose of calculations
        //the format is x1 v1 x2 v2 x3 v3 and so on
        y.resize(length*2);
        k1.resize(length*2);
        k2.resize(length*2);
        k3.resize(length*2);
        k4.resize(length*2);
        intermediate.resize(length*2);

        this->kvalue = kvalue;
        this->restoringK = restoringK;
        this->bvalue = bvalue;
    }

    void tickTime(float deltaT){
        for (int i = 0; i<points.size(); i++){
            //force = a, if we assume each mass to be 1

            //my'' + by'  = Fxt-ky
            //LEGACY CODE
//
//            float netForce = -1 * restoringK * points[i].y;
//
//            if (i!=0){
//                netForce += kvalue * (points[i-1].y - points[i].y);
//            }
//
//            if (i!=points.size()-1){
//                netForce += kvalue * (points[i+1].y - points[i].y);
//            }
//
//            if (i == 0 | i== points.size()-1){
//                netForce =0;
//            }
//
//            netForce -= bvalue * points[i].vy;
//
//            points[i].vy += deltaT * netForce;
//            points[i].y  += deltaT * points[i].vy;
            computeChange(deltaT,i);
        }
        for (int i = 0; i<points.size();i++){
            points[i].y+= pointsdt[i].y;
            points[i].vy+=pointsdt[i].vy;
        }
    }
    //h is deltaT here
    void computeChange(float h,int id){
        //here we start off by computing what we know
        float x = points[id].y; float vx = points[id].vy;
        //this is just f(x,vx)
        Point k1 = computeDerivative(x,vx,id);
        //this should be f(x+hdx/2,vx+hdvx/2
        Point k2 = computeDerivative(x+h*k1.y/2,vx+h*k1.vy/2,id);
        Point k3 = computeDerivative(x+h*k2.y/2,vx+h*k2.vy/2,id);
        Point k4 = computeDerivative(x+h*k3.y,vx+h*k3.vy,id);

        pointsdt[id] = Point{h/6.0f*(k1.y+2*k2.y+2*k3.y+k4.y),h/6.0f*(k1.vy+2*k2.vy+2*k3.vy+k4.vy)};
    }

    float computeDrivingF(float y, int id){
        float netForce = 0;
        if (id!=0){
            netForce += kvalue * (points[id-1].y - points[id].y);
        }
        else{
            netForce += kvalue * (0 - points[id].y);
        }

        if (id!=points.size()-1){
            netForce += kvalue * (points[id+1].y - points[id].y);
        }
        else{
            netForce += kvalue * (0 - points[id].y);
        }

        return netForce;
    }

    Point computeDerivative(float x, float vx,int id){
        return Point{vx, computeDrivingF(x,id)-kvalue*x-bvalue*vx};
    }

    //system and out can't be the same vector
    //this is to save a copy during RK4
    void computeDerivative(const std::vector<float>& system, std::vector<float>& out){
        float x, vx,extF;
        for (int i = 0; 2*i< system.size(); i++){
            //2i is the index of the positition, 2i+1 is the index of the velocity
            //vx should get stored in x and F(x)-kx-bvx shoudl be stored in vx
            x = system[2*i]; vx = system[2*i+1];
            out[2*i] = vx;

            extF = 0;
            if (i!=0){
                extF += kvalue * (system[2*(i-1)] - x);
            }

            else{
                extF += kvalue * (0 - x);
            }

            if (i!=system.size()/2-1){
                extF += kvalue * (system[2*(i+1)] - x);
            }

            else{
                extF += kvalue * (0 - x);
            }

            out[2*i+1] = extF - restoringK * x - bvalue * vx;

        }
    }

    void tickTimeRK4(float h){
        //
        computeDerivative(y,k1);

        for (int i = 0; i < y.size(); i++){
            intermediate[i] = y[i] + h*k1[i]/2;
        }

        computeDerivative(intermediate,k2);

        for (int i = 0; i < y.size(); i++){
            intermediate[i] = y[i] + h*k2[i]/2;
        }

        computeDerivative(intermediate, k3);

        for (int i = 0; i < y.size(); i++){
            intermediate[i] = y[i] + h*k3[i];
        }

        computeDerivative(intermediate, k4);

        for (int i = 0; i < y.size(); i++){
            y[i] += h/6.0f * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
        }
    };


    //we make a triangular pulse?
    //a triangle with base width and height magnitude
    //arithmetic progression from magnitude 0 at x-width/2 up to magntidue at x
    void impulse(int x, float magnitude,int width){
        for (int i =x-width/2; i<=x;i++){
            if (i>=0 && i<points.size()) {
                points[i].vy += magnitude/((float)width/2) * (i-(x-width/2));
            }
        }

        for (int i =x+1; i< x+width/2;i++){
            if (i>=0 && i<points.size()) {
                points[i].vy += magnitude/((float)width/2) * ((x+width/2)-i);
            }
        }

    }

    void newImpulse(int x, float magnitude,int width){
        for (int i =x-width/2; i<=x;i++){
            if (i>=0 && i<points.size()) {
                y[2*i+1] += magnitude/((float)width/2) * (i-(x-width/2));
            }
        }

        for (int i =x+1; i< x+width/2;i++){
            if (i>=0 && i<points.size()) {
                y[2*i+1] += magnitude/((float)width/2) * ((x+width/2)-i);
            }
        }

    }
};

class Wave2D{
private:
    std::vector<float> intermediate;
    std::vector<float> k1;
    std::vector<float> k2;
    std::vector<float> k3;
    std::vector<float> k4;
    std::thread thread_1;
public:
    std::vector<float> y;
    float restoringK;
    float kvalue, bvalue;
    int length,w,h;
    Wave2D(float restoringK, float kvalue, float bvalue, int w, int h){
        this->restoringK = restoringK;
        this->kvalue = kvalue;
        this->bvalue = bvalue;
        this->w = 2*w;
        this->h = h;
        this->length = this->w*h;
        y.resize(length);
        k1.resize(length);
        k2.resize(length);
        k3.resize(length);
        k4.resize(length);
        intermediate.resize(length);
    }

    //in our coordinate system the x value of a point = index % width, and the y value = index / height
    // index = 2*x + y
    //the format goes like this : x1 v1 x2 v2 x3 v3
    //                            x4 v4 x5 v5 x6 v6
    //this represents a board that is 3 wide and 2 tall, but in memory it is stored as a 12 item array

    void tickRK4(float h){
        computeDerivative(&y,&k1);

        for (int i = 0; i < y.size(); i++){
            intermediate[i] = y[i] + h*k1[i]/2;
        }

        computeDerivative(&intermediate,&k2);

        for (int i = 0; i < y.size(); i++){
            intermediate[i] = y[i] + h*k2[i]/2;
        }

        computeDerivative(&intermediate, &k3);

        for (int i = 0; i < y.size(); i++){
            intermediate[i] = y[i] + h*k3[i];
        }

        computeDerivative(&intermediate, &k4);

        for (int i = 0; i < y.size(); i++){
            y[i] += h/6.0f * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
        }
    }

    void computeDerivative( std::vector<float> * system, std::vector<float> * out){

        if (!THREAD_RK4){
            float x, vx, extF;
            for (int j = 0; j < h; j++) {
                for (int i = 0; i < w/2; i++){
                    computePointDerivative(*system,*out,i,j);
                }
            }
        }
        else{
            ThreadParams firstThreadArgs {std::ref(system),std::ref(out),0,h/2-1};
            ThreadParams secondThreadArgs {system,out,h/2,h-1};

            std::thread thread_1(&Wave2D::computeDerivativeBounded,this,&firstThreadArgs);

            computeDerivativeBounded(&secondThreadArgs);
            thread_1.join();


        }


    }
    //has bounds on it, inclusive
    void computeDerivativeBounded(ThreadParams * obj){
        float x, vx, extF;
        for (int j = obj->startY; j <= obj->endY; j++) {
            for (int i = 0; i < w/2; i++){
                computePointDerivative(*obj->system,*obj->out,i,j);
            }
        }
    }

    void computePointDerivative(const std::vector<float> & system, std::vector<float> & out, int px, int py){
        //system[y*w+2x] is the position at the current point
        //system [y*w+2x+1] is the velocity
        float x = getX(system,px,py); float vx = getVx(system,px,py);
        //this sets the derivative of x which is vx
        setX(out,px,py,vx);

        //now we compute the force - it is pulled on by the four squares around it
        float extF = 0;

        if(px!=0){
            extF += kvalue*(getX(system,px-1,py) - x);
        }
        else{
            extF += kvalue*(0 - x);
        }

        if(px!=w/2-1){
            extF += kvalue*(getX(system,px+1,py) - x);
        }
        else{
            extF += kvalue*(0 - x);
        }

        if(py!=0){
            extF += kvalue*(getX(system,px,py-1) - x);
        }
        else{
            extF += kvalue*(0 - x);
        }

        if(py!=h-1){
            extF += kvalue*(getX(system,px,py+1) - x);
        }
        else{
            extF += kvalue*(0 - x);
        }

        //sets the derivative of the velocity
        setVx(out,px,py,extF - restoringK*x - bvalue*vx);
    }

    void impulse(int px, int py, float magnitude, float radius){
        //we iterate through a square with side 2*((int) radius + 1)
        // see if any items in square are <= radius distance away

        for (int x = px - ((int) radius + 1); x<px+((int) radius + 1); x++){
            for (int y = py - ((int) radius + 1); y<py+((int) radius + 1); y++){
                if (distance(x,y,px,py) <= radius){
                    setVx(this->y,x,y,(radius - distance(x,y,px,py) * magnitude));
                }
            }
        }

    }

    inline void setX(std::vector<float> & system, int px, int py, float newx){
        system[py*w + 2*px]  = newx;
//        assert(px<w/2);
    }

    inline float getX(const std::vector<float> & system,int px, int py){
        if (px>w/2 || px<0){
            //greakpoint
        }
        return system[py*w + 2*px];
    }

    inline void setVx(std::vector<float> &  system,int px, int py, float newvx){
        system[py*w + 2*px + 1] = newvx;
        assert(px<w/2);
    }

    inline float getVx(const std::vector<float> & system,int px, int py){
//        assert(px<w/2);
        return system[py*w + 2*px +1];

    }

    inline float distance(float x1, float y1,float x2,float y2){
        return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
    }
};