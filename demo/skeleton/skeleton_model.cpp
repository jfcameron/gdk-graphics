// © Joseph Cameron - All Rights Reserved

#include "skeleton_model.h"

using namespace gdk;

skeleton::skeleton(const std::vector<bone_data> &aBoneData)
{
    std::unordered_map<std::string, std::vector<std::string>> boneNameToChildrenNamesMap;
    std::set<std::string> usedChildrenNames;

    for (auto &currentBoneData : aBoneData)
    {
        if (boneMap.find(currentBoneData.name) != boneMap.end()) 
            throw std::runtime_error("duplicate bone found in skeleton");

        for (const auto &currentChildName : currentBoneData.children)
        {
            if (currentBoneData.name == currentChildName)
                throw std::runtime_error("a bone cannot be its own parent");

            if (usedChildrenNames.find(currentChildName) != usedChildrenNames.end()) 
                throw std::runtime_error("bones cannot have multiple parents");

            usedChildrenNames.insert(currentChildName);
        }

        //Building maps
        boneMap.emplace(
            currentBoneData.name, 
            bone{
                currentBoneData.transform,
                currentBoneData.transform,
                {},
                nullptr
            });

        boneNameToChildrenNamesMap.emplace(
            currentBoneData.name, 
            currentBoneData.children);
    }

    rootBone = nullptr;

    for (auto &[currentBoneName, currentChildrenNames] : boneNameToChildrenNamesMap)
    {
        for (auto &currentChildName : currentChildrenNames)
        {
            //Constructing children ptr collections
            boneMap[currentBoneName].children.insert(&boneMap[currentChildName]);
            
            //Assigning parent ptr to children
            boneMap[currentChildName].parent = &boneMap[currentBoneName];
        }
        
        //Finding root bone and throwing if multiple
        if (usedChildrenNames.find(currentBoneName) == usedChildrenNames.end())
        {
            if (rootBone != nullptr) throw std::runtime_error("skeleton cannot have multiple root bones");

            rootBone = &boneMap[currentBoneName];
        }
    }
}

void skeleton::set_local_transform(std::string aBoneName, graphics_matrix4x4_type aTransform)
{
    if (boneMap.find(aBoneName) == boneMap.end()) throw std::runtime_error("could not find bone");

    bone &targetBone = boneMap[aBoneName];
   
    // Calculate transform from root to this
    {
        bone *pCurrentBone = targetBone.parent;
        std::vector<graphics_matrix4x4_type> trunkMatrices;
        while (pCurrentBone != nullptr)
        {
            trunkMatrices.push_back(pCurrentBone->transform);
            pCurrentBone = pCurrentBone->parent;
        }
        graphics_matrix4x4_type trunkMatrix;
        for (auto it = trunkMatrices.rbegin(); it != trunkMatrices.rend(); ++it)
        {
            trunkMatrix *= *it;
        }
        targetBone.transform = trunkMatrix * targetBone.localTransform * aTransform;
        targetBone.localTransform = aTransform;
    }

    // Apply tranforms to children -> call this recalculate_from? dunno
    {
        const std::function<void(bone *)> transformChildren = [&](bone *pCurrentBone)
        {
            if (pCurrentBone == nullptr) return;

            for (auto *currentChild : pCurrentBone->children) 
            {
                currentChild->transform = pCurrentBone->transform * currentChild->localTransform;

                transformChildren(currentChild);
            }
        };

        transformChildren(&targetBone);
    }
}

skeleton animation::get_frame(const float aTime) const
{
    //const auto &keyFrameSet = ??

    //get the 2 closest frames

    //return interpolation
    return {{}};
}

animation::animation(key_frame_collection_type aKeyFrames)
: m_KeyFrames(aKeyFrames)
{}

