// © Joseph Cameron - All Rights Reserved

#include <gdk/vertex_data.h>

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <stdexcept>

using namespace gdk;

vertex_data::attribute_data::view::view(
    jfc::contiguous_view<const component_type> aDataView,
    const size_t aComponentCount)
: m_pData(aDataView.begin())
, m_DataLength(aDataView.size())
, m_ComponentCount(aComponentCount)
{
    if (!m_DataLength) throw std::invalid_argument("attribute data view must contain data");
    if (!m_ComponentCount) throw std::invalid_argument("attribute component count cannot be zero");
}

vertex_data::attribute_data::view::view(const std::vector<component_type> &aData, const size_t aComponentCount)
: vertex_data::attribute_data::view::view(jfc::contiguous_view<const component_type>(aData), aComponentCount)
{}

vertex_data::attribute_data::attribute_data(
    const std::vector<component_type> &aComponents, 
    const size_t aComponentCount)
: m_Components(aComponents)
, m_ComponentCount(aComponentCount)
//, m_VertexCount(aComponents.size() / aComponentCount)
{}

vertex_data::attribute_data &vertex_data::attribute_data::operator+=(const attribute_data &rhs)
{
    //TODO: THROW IF m_ComponentCount != rhs.m_ComponentCount

    m_Components.reserve(m_Components.size() + rhs.m_Components.size());
    m_Components.insert(m_Components.end(), rhs.m_Components.begin(), rhs.m_Components.end());

    return *this;
}

size_t vertex_data::attribute_data::view::data_size() const { return m_DataLength; }

size_t vertex_data::attribute_data::view::component_count() const { return m_ComponentCount; }
    
const vertex_data::component_type *vertex_data::attribute_data::view::begin() const { return m_pData; }

void vertex_data::overwrite(const size_t index, const vertex_data &other)
{
    for (const auto &[other_name, other_attribute_data] : other.data())
    {
        auto &this_attribute_data = m_NonInterleavedData[other_name];

        if (this_attribute_data.m_ComponentCount != other_attribute_data.m_ComponentCount)
            throw std::invalid_argument("component counts must match");

        auto component_index(index * other_attribute_data.m_ComponentCount);

        if (component_index > this_attribute_data.m_Components.size())
            throw std::invalid_argument("out of range");

        if (component_index + other_attribute_data.m_Components.size() > this_attribute_data.m_Components.size())
            throw std::invalid_argument("out of range");

        std::copy(other_attribute_data.m_Components.begin(), 
            other_attribute_data.m_Components.end(),
            this_attribute_data.m_Components.begin() + component_index);
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

    if (thisAttributeData.m_ComponentCount != thatAttributeData.m_ComponentCount) 
        throw std::invalid_argument("attribute's component count must match");

    const size_t componentOffset(vertexOffset * thisAttributeData.m_ComponentCount);
    
    if (componentOffset + thatAttributeData.m_Components.size() > thisAttributeData.m_Components.size())
        throw std::invalid_argument("new attrib data would write past the end of current attribute data");

    //TODO: overwrite
    std::copy(thatAttributeData.m_Components.begin(), 
        thatAttributeData.m_Components.begin() + thatAttributeData.m_Components.size(),
        thisAttributeData.m_Components.begin() + componentOffset);
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

    m_VertexCount = m_NonInterleavedData.begin()->second.m_Components.size() /
            m_NonInterleavedData.begin()->second.m_ComponentCount;

    return indexToHeadOfNewData;
}

vertex_data::vertex_data(const attribute_name_to_view_type &aAttributeData)
{
    if (!aAttributeData.empty())
    {
        const size_t vertexCount(aAttributeData.begin()->second.data_size() /
            aAttributeData.begin()->second.component_count());

        if (!vertexCount) throw std::invalid_argument("vertex_data: "
            "vertex attribute data must have data");

        for (const auto &[current_name, current_attribute_data_view] : aAttributeData)
        {
            auto currentVertexCount = current_attribute_data_view.data_size() /
                current_attribute_data_view.component_count();

            if (currentVertexCount != vertexCount) throw std::invalid_argument("vertex_data: "
                "attribute data arrays must contribute to the same number of vertexes");
        }

        for (const auto &[current_name, current_attribute_data_view] : aAttributeData)
        {
            const size_t currentAttribVertexCount = current_attribute_data_view.data_size() / current_attribute_data_view.component_count();

            if (currentAttribVertexCount != vertexCount) throw std::invalid_argument(
                "attribute data must contribute to the same number of vertexes");

            vertex_data::attribute_data data(
                std::vector<vertex_data::component_type>(current_attribute_data_view.begin(), 
                    current_attribute_data_view.begin() + current_attribute_data_view.data_size()),
                current_attribute_data_view.component_count());

            m_NonInterleavedData.emplace(current_name, data);
        }

        m_VertexCount = vertexCount;
    }
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
#include <iostream>
void vertex_data::sort_faces(
    const std::function<bool(graphics_vector3_type /*first vertex*/, 
        graphics_vector3_type /*second vertex*/)> &aComparator,
    const std::string &aPositionAttributeName)
{
    if (auto search = m_NonInterleavedData.find(aPositionAttributeName); search != m_NonInterleavedData.end())
    {
        auto &position_attribute_data = search->second.m_Components;

        struct triangle {
            size_t index;
            
            graphics_vector3_type a;
        };
        std::vector<triangle> triangles;

        size_t i(0);
        for (auto p = position_attribute_data.begin(); p != position_attribute_data.end(); p += 9)
        {
            auto x1(p + 0), y1(p + 1), z1(p + 2);
            auto x2(p + 3), y2(p + 4), z2(p + 5);
            auto x3(p + 6), y3(p + 7), z3(p + 8);

            triangles.push_back({
                .index = i,
                /*.a = {*x1,*y1,*z1},
                .b = {*x2,*y2,*z2},
                .c = {*x3,*y3,*z3}*/
                
                .a = {*x1,*y1,*z1}, //TODO: make this the centroid
            });

            i += 3;
        }

        std::cout << "======================================\n";
        for (auto &tri : triangles) std::cout << tri.index << "\n";

        /*std::sort(triangles.begin(), triangles.end(),
            [](triangle a, triangle b)
            {
                const auto cameraPos = graphics_vector3_type(0.f,0.f,-30.f); //hardcoded camera pos
                const auto entityPosA = a.a;
                const auto entityPosB = b.a;

                const auto aDist = cameraPos.distance(entityPosA);
                const auto bDist = cameraPos.distance(entityPosB);

                return (aDist < bDist);
            });*/

        std::cout << ",,,,,,,,,,,,,,,,,,,,\n";
        for (auto &tri : triangles) std::cout << tri.index << "\n";
   
        i = 0;
        for (auto triangle : triangles)
        {
            /*auto p = position_attribute_data.begin() + (i * 9);
            
            auto x1(p + 0), y1(p + 1), z1(p + 2);
            auto x2(p + 3), y2(p + 4), z2(p + 5);
            auto x3(p + 6), y3(p + 7), z3(p + 8);

            *x1 = triangle.a.x;
            *y1 = triangle.a.y;
            *z1 = triangle.a.z;

            *x2 = triangle.b.x;
            *y2 = triangle.b.y;
            *z2 = triangle.b.z;

            *x3 = triangle.c.x;
            *y3 = triangle.c.y;
            *z3 = triangle.c.z;*/
            
            auto p = position_attribute_data.begin() + (i * 9);

            auto x1(p + 0), y1(p + 1), z1(p + 2);
            auto x2(p + 3), y2(p + 4), z2(p + 5);
            auto x3(p + 6), y3(p + 7), z3(p + 8);

            float buffer;
            auto begin = position_attribute_data.begin() + triangle.index;

            std::cout << *x1 << " vs " << *(begin + 0) << "\n";
            /*std::cout << *y2 << " vs " << *(begin + 1) << "\n";
            std::cout << *z3 << " vs " << *(begin + 2) << "\n";
            std::cout << *x2 << " vs " << *(begin + 3) << "\n";*/

            /*buffer = *x1; *x1 = *(begin + triangle.index1 + 0); *(begin + triangle.index1 + 0) = buffer;
            buffer = *y1; *y1 = *(begin + triangle.index1 + 1); *(begin + triangle.index1 + 1) = buffer;
            buffer = *z1; *z1 = *(begin + triangle.index1 + 2); *(begin + triangle.index1 + 2) = buffer;

            buffer = *x2; *x2 = *(begin + triangle.index1 + 3); *(begin + triangle.index1 + 3) = buffer;
            buffer = *y2; *y2 = *(begin + triangle.index1 + 4); *(begin + triangle.index1 + 4) = buffer;
            buffer = *z2; *z2 = *(begin + triangle.index1 + 5); *(begin + triangle.index1 + 5) = buffer;
            
            buffer = *x3; *x3 = *(begin + triangle.index1 + 6); *(begin + triangle.index1 + 6) = buffer;
            buffer = *y3; *y3 = *(begin + triangle.index1 + 7); *(begin + triangle.index1 + 7) = buffer;
            buffer = *z3; *z3 = *(begin + triangle.index1 + 8); *(begin + triangle.index1 + 8) = buffer;*/

            ++i;
        }
    }
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
        auto &position_attribute_data = search->second.m_Components;

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
        auto &uv_attribute_data = search->second.m_Components;

        for (auto p = uv_attribute_data.begin(); p != uv_attribute_data.end(); p += 2) //TODO because iters, MUST BE CERTAIN OF COMPONENET COUNT!
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

