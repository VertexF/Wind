#ifndef FORCEGEN_H_INCLUDED
#define FORCEGEN_H_INCLUDED
#include <vector>
#include "body.h"
#include "pfgen.h"

#include <iostream>

/** Force Generator for rigid bodies.  */

namespace wind
{
    class ForceGenerator
    {
        public:
            //Each force generator class must implement the update force function.
            virtual void updateForce(RigidBody *body, real duration) = 0;
    };

    class Gravity : public ForceGenerator
    {
        Vector3 gravity;

    public:
        //Sets up the gravity vector force.
        Gravity(const Vector3 &grav);

        //This function applies the a gravity force to a rigid body.
        virtual void updateForce(RigidBody *body, real duration);
    };

    /** This force generator handles engine propulsion forces */
    class Propulsion : public ForceGenerator
    {
        bool move;

        public:
            Propulsion(const Vector3 &prop, const Vector3 &pos, const Vector3 *wind);

            virtual void updateForce(RigidBody *body, real duration);

        protected:
            const Vector3 *windspeed;

            Vector3 position;

            Vector3 propulsion;
    };

    /**
        This class supports force generated for rigid body buoyancy force.
    */
    class Buoyancy : public ForceGenerator
    {
        private:
            //This contains the max depths the rigid body will sink into the liquid
            real maxDepth;

            //This contains the volume of the rigid body.
            real volume;

            //This tells the force how high the water is.
            real waterHeight;

            //This contains how dense the liquid is.
            real liquidDensity;

            //This is the point of the centre of buoyancy for the rigid body.
            Vector3 centreOfBuoyancy;
        public:
            //This sets up the force direction and the liquid information.
            //The liquid density has the default value of the liquid density of water.
            Buoyancy(const Vector3& cOfB, real maxDepth, real volume, real waterHeight, real liquidDensity = 1000.f);

            //Updates the force.
            virtual void updateForce(RigidBody* body, real duration);
    };


    /** This force generator class will hold information for aerodynamics for things that fly.
        This class doesn't hold all the information that is involved in aerodynamics because that is too complex.
        A aero tensor which is 3 by 3 will hold the information which will be multiplied by the air velocity to give the final force.
     */
    class Aero : public ForceGenerator
    {
        public:
            //The construct sets everything up.
            Aero(const Matrix3 &ten, const Vector3 &pos, const Vector3 *wind);

            //This function will calculate and update the force to a body.
            virtual void updateForce(RigidBody *body, real duration);

        protected:
            //This holds the tensor which is used to hold the aerodynamics information of the surface of the body in space.
            Matrix3 Tensor;

            //This will hold the relative position of the aerodynamic surface in local space.
            Vector3 position;

            //Here we have a pointer vector which will hold the wind speed.
            //It's a pointer to have a passed in world speed rather than calculating the wind speed in each force generator
            const Vector3 *windspeed;

            //This function is the same as the update force but we use the arguments aerodynamic tensor to update the force.
            void updateForceFromTensor(RigidBody *body, real duration, const Matrix3 &tensor);
    };

    /** In this class we the Aero class techniques to build come control into flying
        Here we will extend this by using three aerodynamic tensors to better represent the surface aerodynamics.
        Two tensors will be the most extreeeeeeeme(hash and slash title) versions of resting positions of the control surface.
        The final tensor will be inherited from the base class.
    */
    class AeroControl : public Aero
    {
        public:
            //The constructor starts up everything.
            AeroControl(const Matrix3 &base, const Matrix3 &min, const Matrix3 &max, const Vector3 &pos, const Vector3 *wind);

            //This function set up the controls which will only handle values -1, 0 and 1 and anything outside this range will be ignored.
            void setControl(real value);

            // This function applies the force to the rigid body.
            virtual void updateForce(RigidBody *body, real duration);

        protected:
            //This tensor will hold the aerodynamic maximum "resting" value position for the surface.
            Matrix3 maxTensor;

            //This tensor will hold the aerodynamic minimum "resting" value position for the surface.
            Matrix3 minTensor;
       
            //This value will have a range from -1 to 0 to 1. Each value tell use which tensor we should use.
            //Were -1 we would use the minTensor, 0 the base tensor and 1 we will use the maxTensor.
            real controlSettings;
        private:
            //The function returns the final tensor aerodynamic for the current setting.
            Matrix3 getTensor();
    };

    class AngledAero : public Aero
    {

    };

    class Spring : public ForceGenerator
    {
        //The connection point to a rigid body in local space.
        Vector3 connectionPoint;

        //The other end of the spring connection
        RigidBody *other;

        //The point of connection to another rigid body in local space.
        Vector3 otherConnectionPoint;

        //The spring constant.
        real springConstant;

        //The spring length
        real restLength;

        public:
            //Sets up starts up all the values.
            Spring(const Vector3 &localConnection, RigidBody *other, const Vector3 &otherConnection, real springConst, real restLen);

            //Adds the spring force
            virtual void updateForce(RigidBody *body, real duration);
    };

     //When building a Force Generator you need to designed something to store away what force has been applied to what rigid body.
    //This is what this class does is it holds all the force generated by the force generator and stores them away, along with the rigid body it applies to.
    class ForceRegistry
    {
        protected:

            //This struct will keep track of all the rigid body force generations.
            struct ForceRegistration
            {
                RigidBody* mBody;
                ForceGenerator* mFG;
            };

            //This is a type defined vector that makes a type of a ForceRegistration.
            //This will hold a list of the ForceRegistration in a vector.
            typedef std::vector<ForceRegistration> Registry;
            //Of course when we define a type we have to give it a instance.
            Registry mRegistration;
        public:
            //Registers a force need to be added to a rigid body .
            void add(RigidBody* body, ForceGenerator* FG);
            //Removes the registered rigid body and the force generator.
            //Note: That if the rigid body force generator and a rigid body are not linked this will have no affect.
            void remove(RigidBody* body, ForceGenerator* FG);
            //Clears only the register of the rigid bodies and there generator.
            //Note: This will not delete the rigid body or the rigid body force generator just the registration of the two.
            void clear();
            //This function calls the force generator to updates the force of the corresponding rigid body.
            void updateForce(real duration, RigidBody* body);
    };
};

#endif // FORCEGEN_H_INCLUDED
