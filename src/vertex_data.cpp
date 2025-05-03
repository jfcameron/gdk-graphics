// © Joseph Cameron - All Rights Reserved

#include <gdk/vertex_data.h>

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <stdexcept>

using namespace gdk;

vertex_data::attribute_data::attribute_data(
    const std::vector<component_type> &aComponents, 
    const size_t aComponentCount)
: m_Components(aComponents)
, m_ComponentCount(aComponentCount)
{}

vertex_data::attribute_data &vertex_data::attribute_data::operator+=(const attribute_data &rhs)
{
    //TODO: THROW IF m_ComponentCount != rhs.m_ComponentCount

    m_Components.reserve(m_Components.size() + rhs.m_Components.size());
    m_Components.insert(m_Components.end(), rhs.m_Components.begin(), rhs.m_Components.end());

    return *this;
}

size_t vertex_data::attribute_data::component_count() const { return m_ComponentCount; }

std::vector<vertex_data::component_type> &vertex_data::attribute_data::components() { return m_Components; }

const std::vector<vertex_data::component_type> &vertex_data::attribute_data::components() const { return m_Components; }

void vertex_data::overwrite(const size_t index, const vertex_data &other)
{
    for (const auto &[other_name, other_attribute_data] : other.data())
    {
        auto &this_attribute_data = m_NonInterleavedData[other_name];

        if (this_attribute_data.component_count() != other_attribute_data.component_count())
            throw std::invalid_argument("component counts must match");

        auto component_index(index * other_attribute_data.component_count());

        if (component_index > this_attribute_data.components().size())
            throw std::invalid_argument("out of range");

        if (component_index + other_attribute_data.components().size() > this_attribute_data.components().size())
            throw std::invalid_argument("out of range");

        std::copy(other_attribute_data.components().begin(), 
            other_attribute_data.components().end(),
            this_attribute_data.components().begin() + component_index);
    }
}

void vertex_data::overwrite(const std::string &aAttributeName, const size_t vertexOffset, const vertex_data &other)
{
    auto search(m_NonInterleavedData.find(aAttributeName));
    
    if (search == m_NonInterleavedData.end()) throw std::invalid_argument("attribute not present");

    auto &thisAttributeData = search->second;

    //

    auto search2(other.m_NonInterleavedData.find(aAttributeName));
    
    if (search2 == other.m_NonInterleavedData.end()) throw std::invalid_argument("attribute not present");

    const auto &thatAttributeData = search2->second;

    if (thisAttributeData.component_count() != thatAttributeData.component_count()) 
        throw std::invalid_argument("attribute's component count must match");

    const size_t componentOffset(vertexOffset * thisAttributeData.component_count());
    
    if (componentOffset + thatAttributeData.components().size() > thisAttributeData.components().size())
        throw std::invalid_argument("new attrib data would write past the end of current attribute data");

    //TODO: overwrite
    std::copy(thatAttributeData.components().begin(), 
        thatAttributeData.components().begin() + thatAttributeData.components().size(),
        thisAttributeData.components().begin() + componentOffset);
}

//TODO: unify push_back and vertex_data::vertex_data. likely need a data to view converter etc.
size_t vertex_data::push_back(const vertex_data &other)
{
    if (other.m_NonInterleavedData.empty()) return m_VertexCount;

    if (m_NonInterleavedData.empty()) 
    {   
        (*this) = other;

        return m_VertexCount;
    }

    if (other.m_PrimitiveMode != m_PrimitiveMode) throw 
        std::invalid_argument("vertex_data: "
            "incoming primitive modes must match");

    //TODO: Assert that the vertex_data's attribute names and components per attrib match
    /*if (other.m_Format != m_Format) throw 
        std::invalid_argument("vertex_data: "
            "incoming attribute format must match");*/
    
    for (const auto &[other_name, other_attribute_data] : other.data())
    {
        m_NonInterleavedData[other_name] += other_attribute_data;
    }

    size_t indexToHeadOfNewData(m_VertexCount);

    m_VertexCount = m_NonInterleavedData.begin()->second.components().size() /
            m_NonInterleavedData.begin()->second.component_count();

    return indexToHeadOfNewData;
}

vertex_data::vertex_data(attribute_collection_type &&aAttributeData)
{
    const size_t vertexCount(aAttributeData.begin()->second.components().size() /
        aAttributeData.begin()->second.component_count());

    for (const auto &[current_name, current_attribute_data_view] : aAttributeData)
    {
        const size_t currentAttribVertexCount = current_attribute_data_view.components().size() / current_attribute_data_view.component_count();

        if (currentAttribVertexCount != vertexCount) throw std::invalid_argument(
            "attribute data must contribute to the same number of vertexes");
    }

    m_NonInterleavedData = aAttributeData;
    m_VertexCount = vertexCount;
}

vertex_data::PrimitiveMode vertex_data::primitive_mode() const 
{ 
    return m_PrimitiveMode; 
}

vertex_data &vertex_data::operator+=(const vertex_data &other) 
{ 
    push_back(other); 

    return *this;
}

vertex_data vertex_data::operator+(const vertex_data &aRightHand)
{
    vertex_data newVertexData(*this);

    newVertexData += aRightHand;

    return newVertexData;
}

void vertex_data::clear()
{
    m_NonInterleavedData.clear();
}

const std::vector<vertex_data::index_value_type> &vertex_data::getIndexData() const 
{ 
    //TODO: need to add "index_data_view" as an optional param for vertex_data
    static const std::vector<vertex_data::index_value_type> index_data;
    return index_data;
}

struct triangle {
    size_t index;
    graphics_vector3_type centroid;
};

void sort_by_triangle(
    std::function<bool(triangle, triangle)> aSort,
    vertex_data &aVertexData,
    const std::string &aPositionAttributeName)
{
    auto m_NonInterleavedData = aVertexData.data();

    if (auto search = m_NonInterleavedData.find(aPositionAttributeName); search != m_NonInterleavedData.end())
    {
        auto &position_attribute_data = search->second.components();

        std::vector<triangle> triangles;

        size_t i(0);
        for (auto p = position_attribute_data.begin(); p != position_attribute_data.end(); p += 9)
        {
            auto x1(p + 0), y1(p + 1), z1(p + 2);
            auto x2(p + 3), y2(p + 4), z2(p + 5);
            auto x3(p + 6), y3(p + 7), z3(p + 8);

            triangles.push_back({
                i,
                {
                    (*x1 + *x2 + *x3) / vertex_data::component_type(3.f),
                    (*y1 + *y2 + *y3) / vertex_data::component_type(3.f),
                    (*z1 + *z2 + *z3) / vertex_data::component_type(3.f)
                }, 
            });

            i += 3;
        }
       
        std::sort(triangles.begin(), triangles.end(), aSort);

        decltype(m_NonInterleavedData) newNonInterleavedData;

        for (auto &[key, value] : m_NonInterleavedData)
        {
            const auto currentComponentCount = value.component_count();
            auto &currentAttributeData = value.components();

            std::vector<vertex_data::component_type> new_attribute_data;
            new_attribute_data.reserve(currentAttributeData.size());
       
            size_t triangleIndex = 0;
            for (auto triangle : triangles)
            {
                auto pSortedTriangle = currentAttributeData.begin() + (triangle.index * currentComponentCount);

                for (size_t componentIndex = 0; componentIndex < 3 * currentComponentCount; ++componentIndex)
                {
                    auto newComponentPtr = pSortedTriangle + componentIndex;

                    new_attribute_data.push_back(*newComponentPtr);
                }

                ++triangleIndex;
            }
            
            newNonInterleavedData[key] = {
                std::move(new_attribute_data),
                currentComponentCount
            };
        }

        aVertexData = vertex_data(std::move(newNonInterleavedData));
    }
}

void vertex_data::sort_by_nearest_triangle(
    const graphics_vector3_type &aObserverWorldPosition,
    graphics_mat4x4_type aEntityInstanceWorldMatrix,
    const std::string &aObserverWorldPositionAttributeName)
{
    //TODO: name becomes misleading at this point. maybe require teh inverse in the method param
    aEntityInstanceWorldMatrix.inverse(); 

    graphics_mat4x4_type observerWorldMatrix;
    observerWorldMatrix.translate(aObserverWorldPosition);
    
    graphics_mat4x4_type localObserver = aEntityInstanceWorldMatrix * observerWorldMatrix;
    graphics_vector3_type observerLocalPostion(localObserver.m[3][0], localObserver.m[3][1], localObserver.m[3][2]);

    sort_by_triangle(
        [&observerLocalPostion](triangle a, triangle b)
        {
            const auto cameraPos = observerLocalPostion;
            const auto entityPosA = a.centroid;
            const auto entityPosB = b.centroid;

            const auto aDist = cameraPos.distance(entityPosA);
            const auto bDist = cameraPos.distance(entityPosB);

            return (aDist < bDist);
        },
        *this,
        aObserverWorldPositionAttributeName);
}

void vertex_data::sort_by_furthest_triangle(
    const graphics_vector3_type &aObserverWorldPosition,
    graphics_mat4x4_type aEntityInstanceWorldMatrix,
    const std::string &aObserverWorldPositionAttributeName)
{
    //TODO: name becomes misleading at this point. maybe require teh inverse in the method param
    aEntityInstanceWorldMatrix.inverse(); 

    graphics_mat4x4_type observerWorldMatrix;
    observerWorldMatrix.translate(aObserverWorldPosition);
    
    graphics_mat4x4_type localObserver = aEntityInstanceWorldMatrix * observerWorldMatrix;
    graphics_vector3_type observerLocalPostion(localObserver.m[3][0], localObserver.m[3][1], localObserver.m[3][2]);

    sort_by_triangle(
        [&observerLocalPostion](triangle a, triangle b)
        {
            const auto cameraPos = observerLocalPostion;
            const auto entityPosA = a.centroid;
            const auto entityPosB = b.centroid;

            const auto aDist = cameraPos.distance(entityPosA);
            const auto bDist = cameraPos.distance(entityPosB);

            return (aDist > bDist);
        },
        *this,
        aObserverWorldPositionAttributeName);
}

void vertex_data::transform_position(
    const graphics_vector3_type &aPos,
    const graphics_quaternion_type &aRot,
    const graphics_vector3_type &aScale,
    const std::string &aPositionAttributeName)
{
    //TODO: throw if attribname isnt there
    //TODO: throw if size isnt 3
    auto rot_euler(aRot.toEuler());

    if (auto search = m_NonInterleavedData.find(aPositionAttributeName); search != m_NonInterleavedData.end())
    {
        auto &position_attribute_data = search->second.components();

        for (auto p = position_attribute_data.begin(); p != position_attribute_data.end(); p += 3)
        {
            auto x(p + 0), y(p + 1), z(p + 2);

            // scale
            *x *= aScale.x;
            *y *= aScale.y;
            *z *= aScale.z;

            // rotate around x
            auto q = rot_euler.x;
            auto _x = *x, _y = *y, _z = *z;
            *y = (_y * std::cos(q)) - (_z * std::sin(q));
            *z = (_y * std::sin(q)) + (_z * std::cos(q));
            *x = _x;

            // rotate around y
            q = rot_euler.y;
            _x = *x, _y = *y, _z = *z;
            *x = (_x * std::cos(q)) - (_z * std::sin(q));
            *z = (_x * std::sin(q)) + (_z * std::cos(q));
            *y = _y;

            // rotate around z
            q = rot_euler.z;
            _x = *x, _y = *y, _z = *z;
            *x = (_x * std::cos(q)) - (_y * std::sin(q));
            *y = (_x * std::sin(q)) + (_y * std::cos(q));
            *z = _z;

            // translate
            *x += aPos.x;
            *y += aPos.y;
            *z += aPos.z;
        }
    }
    //else throw
}

void vertex_data::transform_uv(
    const graphics_vector2_type &aPos,
    const graphics_vector2_type &aScale,
    const std::string &aUVAttributeName)
{
    //TODO: throw if attribname isnt there
    //TODO: throw if size isnt 2
    if (auto search = m_NonInterleavedData.find(aUVAttributeName); search != m_NonInterleavedData.end())
    {
        auto &uv_attribute_data = search->second.components();

        for (auto p = uv_attribute_data.begin(); p != uv_attribute_data.end(); p += 2) //TODO because iters, MUST BE CERTAIN OF COMPONENT COUNT!
        {
            auto x(p + 0), y(p + 1);

            // scale
            *x *= aScale.x;
            *y *= aScale.y;

            /*// rotate around z, centered.
            *x -= 0.5f; *y -= 0.5f;
            auto q = aRot;
            auto _x = *x, _y = *y;
            *x = (_x * std::cos(q)) - (_y * std::sin(q));
            *y = (_y * std::cos(q)) + (_x * std::sin(q));
            *x += 0.5f; *y += 0.5f;*/

            // translate
            *x += aPos.x;
            *y += aPos.y;
        }
    }
}

const vertex_data::attribute_collection_type &vertex_data::data() const
{
    return m_NonInterleavedData;
}

size_t vertex_data::vertex_count() const
{
    return m_VertexCount;
}

