#ifndef COLLISION_BROAD_H
#define COLLISION_BROAD_H
#include <memory>
#include "../include/Body.h"


namespace wind
{
    struct PotentialContact
    {
        //This structure hold the number of bodies that might be in contact
        RigidBody* body[2];
    };

    /**
        This class support the bounding overlap of the board collision by using spheres for each node to see if they over lap
    */
    struct BoundingSphere
    {
        //Basic sphere information.
        Vector3 centre;
        real radius;

        //Creates a new bounds sphere with the give centre point and radius
        BoundingSphere(const Vector3 &centre, real radius);

        //This function creates a bounding sphere to enclose the two given bounding sphere.
        BoundingSphere(const BoundingSphere &first, const BoundingSphere &second);

        //This function checks to see if any of the sphere are overlapping.
        bool overlap(const BoundingSphere *other) const;

        /**
            This function returns the growth of the other bound sphere.
            Note that this calculation returns a value not in any particular units (i.e. its not a volume growth).
            In fact the best implementation takes into account the growth in surface area.
            After the Goldsmith-Salmon algorithm for tree construction has been used.
        */
        real getGrowth(const BoundingSphere &other) const;

	    /**
			This function returns the volume of this bounding volume. This is used
			to calculate how to recurse into the bounding volume tree.
			For a bounding sphere it is a simple calculation.
        */
        real getSize() const
        {
            return ((real)1.333333) * R_PI * radius * radius * radius;
        }
    };

    /**
        This class template is a binary tree data structure to hold the bounding volume hierarchy.
        Each child will be a instance to the class object BondVolumeNode
    */
    template<class BoundingVolumeClass>
    class BondVolumeNode
    {
        public:
            //This will hold the 2 child nodes.
            BondVolumeNode* child[2];

            //This is the root node of the binary tree
            BoundingVolumeClass root;

            //Holds the rigid body at this node of the hierarchy.
            //If we are at the bottom of the binary tree the body is null
            RigidBody* body;

            //This stores the data for the parent node directly above the current node.
            BondVolumeNode* parent;

            //Holds the first and single bounding volume for the root node and encapsulates all other node volumes.
            BoundingVolumeClass volume;

            BondVolumeNode(BondVolumeNode *parent, const BoundingVolumeClass &volume, RigidBody* body = NULL);

            /**
                The deconstructor is really important here. When we delete a rigid body in we need to recalculate the tree to reconsider the number it's bound volume.
            */
            ~BondVolumeNode();

            //Check to see if we are at the bottom of the tree node.
            bool isLeaf() const;

            //This potential contacts.
            unsigned getPotentialContact(PotentialContact* contacts, unsigned int limit) const;


            /**
                This function handles build a board collision hierarchy for the tree.
                In this case it will be using the insertion method of creating a collision hierarchy.
                This allows us to run this in game and keep the collision flexible.
            */
            void insert(RigidBody* newBody, const BoundingVolumeClass &newVolume);

			protected:

				unsigned getPotentialContact(const BoundingVolumeClass* other, PotentialContact* contacts, unsigned int limit) const;

				bool overlaps(const BoundingVolumeClass* other);

				//This function recalculates bounding volume.
				void recalculatesBoundingVolume(bool recursion);
    };
};

#endif // COLLISION_BROAD_H
