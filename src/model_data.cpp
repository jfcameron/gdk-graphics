// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics_exception.h>
#include <gdk/model_data.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace gdk;

model_data model_data::make_quad() {
    return {{ 
        { "a_Position", { {
            1.0f, 1.0f, 0.0f, 
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f,

            1.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
        }, 3 } },
        { "a_UV", { {
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f,

            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
        }, 2 } }
    }};
}

static constexpr auto COMPONENTS_PER_2D_ATTRIBUTE(2);
static constexpr auto COMPONENTS_PER_3D_ATTRIBUTE(3);

attribute_data &model_data::get_attribute_data(const std::string &aAttributeName) {
    auto result(m_Attributes.find(aAttributeName));
    
    if (result == m_Attributes.end()) throw graphics_exception("attribute not present");

    return result->second;
}

void model_data::overwrite(const size_t index, const model_data &otherVertexData) {
    for (const auto &[otherAttributeName, otherAttributeData] : otherVertexData.attributes()) {
        auto &thisAttributeData = get_attribute_data(otherAttributeName);
        thisAttributeData.overwrite(index, otherAttributeData);
    }
}

const attribute_data &model_data::get_attribute_data(const std::string &aAttributeName) const {
    return get_attribute_data(aAttributeName);
}

void model_data::overwrite(const std::string &aAttributeName, const size_t vertexOffset, const model_data &other) {
    auto thisAttributeData = get_attribute_data(aAttributeName);
    auto thatAttributeData = other.get_attribute_data(aAttributeName);

    thisAttributeData.overwrite(vertexOffset, thatAttributeData);
}

void model_data::push_back(const model_data &other) {
    if (other.m_Attributes.empty()) return;

    if (m_Attributes.empty()) {   
        (*this) = other;
        return; 
    }
    
    for (const auto &[OtherAttributeName, otherAttributeData] : other.attributes())
        m_Attributes[OtherAttributeName].push_back(otherAttributeData);

    m_VertexCount = m_Attributes.begin()->second.components().size() / 
        m_Attributes.begin()->second.number_of_components_per_attribute();
}

model_data::model_data(attribute_collection_type &&aAttributeData) {
    const size_t numberOfUniformsFoundInFirstEntry(aAttributeData.begin()->second.number_of_attributes_in_component_data());
    for (const auto &[name, data] : aAttributeData) 
        if (data.number_of_attributes_in_component_data() != numberOfUniformsFoundInFirstEntry) 
            throw graphics_exception("the uniform data found in a vertex data's uniform collection must "
                "have the same number of uniforms present in their component data lists");

    m_Attributes = aAttributeData;
    m_VertexCount = numberOfUniformsFoundInFirstEntry;
}

model_data::primitive_mode model_data::get_primitive_mode() const { return m_PrimitiveMode; }

model_data &model_data::operator+=(const model_data &other) { 
    push_back(other); 

    return *this;
}

model_data model_data::operator+(const model_data &aRightHand) {
    model_data newVertexData(*this);

    newVertexData += aRightHand;

    return newVertexData;
}

void model_data::clear() {
    *this = model_data();
}

const std::vector<model_data::index_value_type> &model_data::indexes() const { 
    static const std::vector<model_data::index_value_type> INDEX_DATA;
    return INDEX_DATA;
}

struct triangle {
    size_t index;
    graphics_vector3_type centroid;
};

//TODO: this should be part of the public interface, so users can do their own triangle based sorting if needed
static void sort_by_triangle(
    std::function<bool(triangle, triangle)> aSorter,
    model_data &aVertexData,
    const std::string &aPositionAttributeName) {
    auto m_Attributes = aVertexData.attributes();

    if (auto search = m_Attributes.find(aPositionAttributeName); search != m_Attributes.end()) {
        auto &position_attribute_data = search->second.components();

        std::vector<triangle> triangles;

        size_t i(0);
        for (auto p = position_attribute_data.begin(); p != position_attribute_data.end(); p += 9) {
            auto x1(p + 0), y1(p + 1), z1(p + 2);
            auto x2(p + 3), y2(p + 4), z2(p + 5);
            auto x3(p + 6), y3(p + 7), z3(p + 8);

            triangles.push_back({
                i,
                {
                    (*x1 + *x2 + *x3) / component_type(3.f),
                    (*y1 + *y2 + *y3) / component_type(3.f),
                    (*z1 + *z2 + *z3) / component_type(3.f)
                }, 
            });

            i += 3;
        }
       
        std::sort(triangles.begin(), triangles.end(), aSorter);

        decltype(m_Attributes) newAttributes;

        for (auto &[key, value] : m_Attributes) {
            const auto currentComponentCount = value.number_of_components_per_attribute();
            auto &currentAttributeData = value.components();

            std::vector<component_type> new_attribute_data;
            new_attribute_data.reserve(currentAttributeData.size());
       
            size_t triangleIndex = 0;
            for (auto triangle : triangles) {
                auto pSortedTriangle = currentAttributeData.begin() + (triangle.index * currentComponentCount);

                for (size_t componentIndex = 0; componentIndex < 3 * currentComponentCount; ++componentIndex) {
                    auto newComponentPtr = pSortedTriangle + componentIndex;

                    new_attribute_data.push_back(*newComponentPtr);
                }

                ++triangleIndex;
            }
            
            newAttributes[key] = {
                std::move(new_attribute_data),
                currentComponentCount
            };
        }

        aVertexData = model_data(std::move(newAttributes));
    }
}

void model_data::sort_by_nearest_triangle(
    const graphics_vector3_type &aObserverWorldPosition,
    graphics_matrix4x4_type aEntityInstanceWorldMatrix,
    const std::string &aObserverWorldPositionAttributeName) {
    //TODO: name becomes misleading at this point. 
    aEntityInstanceWorldMatrix.inverse_affine(); 

    graphics_matrix4x4_type observerWorldMatrix;
    observerWorldMatrix.set_translation(aObserverWorldPosition);
    
    graphics_matrix4x4_type localObserver = aEntityInstanceWorldMatrix * observerWorldMatrix;
    graphics_vector3_type observerLocalPostion(localObserver.m[3][0], localObserver.m[3][1], localObserver.m[3][2]);

    sort_by_triangle(
        [&observerLocalPostion](triangle a, triangle b) {
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

void model_data::sort_by_furthest_triangle(
    const graphics_vector3_type &aObserverWorldPosition,
    graphics_matrix4x4_type aEntityInstanceWorldMatrix,
    const std::string &aObserverWorldPositionAttributeName) {
    //TODO: name becomes misleading at this point. maybe require teh inverse in the method param
    aEntityInstanceWorldMatrix.inverse_affine(); 

    graphics_matrix4x4_type observerWorldMatrix;
    observerWorldMatrix.set_translation(aObserverWorldPosition);
    
    graphics_matrix4x4_type localObserver = aEntityInstanceWorldMatrix * observerWorldMatrix;
    graphics_vector3_type observerLocalPostion(localObserver.m[3][0], localObserver.m[3][1], localObserver.m[3][2]);

    sort_by_triangle(
        [&observerLocalPostion](triangle a, triangle b) {
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

void model_data::transform(const std::string &aPositionAttributeName, graphics_matrix4x4_type &aTransform) {
    auto &attributeData = get_attribute_data(aPositionAttributeName);
    if (attributeData.number_of_components_per_attribute() != COMPONENTS_PER_3D_ATTRIBUTE) 
        throw graphics_exception(std::string("model_data::transform expected 3 components per uniform: ") + aPositionAttributeName);

    auto &components = attributeData.components();

    graphics_vector3_type vectorBuffer;

    for (auto p = components.begin(); p != components.end(); p += COMPONENTS_PER_3D_ATTRIBUTE) {
        auto x(p + 0), y(p + 1), z(p + 2);
        vectorBuffer.x = *x;
        vectorBuffer.y = *y;
        vectorBuffer.z = *z;

        vectorBuffer *= aTransform;

        *x = vectorBuffer.x;
        *y = vectorBuffer.y;
        *z = vectorBuffer.z;
    }
}

void model_data::transform(const std::string &aPositionAttributeName,
    const graphics_vector3_type &aPos,
    const graphics_quaternion_type &aRot,
    const graphics_vector3_type &aScale) {

    graphics_matrix4x4_type mat;
    mat.set_rotation(aRot, aScale);
    mat.set_translation(aPos);
    transform(aPositionAttributeName, mat);

    /*auto &attributeData = get_attribute_data(aPositionAttributeName);
    if (attributeData.number_of_components_per_attribute() != COMPONENTS_PER_3D_ATTRIBUTE) 
        throw graphics_exception("model_data::transform expected 3 components per uniform");

    auto &components = attributeData.components();

    auto rot_euler(aRot.toEuler());

    for (auto p = components.begin(); p != components.end(); p += COMPONENTS_PER_3D_ATTRIBUTE) {
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
    }*/
}

void model_data::transform(const std::string &a2DAttributeName,
    const graphics_vector2_type &aPos, const float aRotation,
    const graphics_vector2_type &aScale) {
    auto &attributeData = get_attribute_data(a2DAttributeName);
    if (attributeData.number_of_components_per_attribute() != COMPONENTS_PER_2D_ATTRIBUTE) 
        throw graphics_exception("model_data::transform expected 2 components per uniform");

    auto &components = attributeData.components();

    for (auto p = components.begin(); p != components.end(); p += COMPONENTS_PER_2D_ATTRIBUTE) {
        auto x(p + 0), y(p + 1);

        // scale
        *x *= aScale.x;
        *y *= aScale.y;

        // rotate around z
        if (aRotation != 0) {
            auto _x = *x, _y = *y;
            *x = (_x * std::cos(aRotation)) - (_y * std::sin(aRotation));
            *y = (_y * std::cos(aRotation)) + (_x * std::sin(aRotation));
        }

        // translate
        *x += aPos.x;
        *y += aPos.y;
    }
}

const model_data::attribute_collection_type &model_data::attributes() const { return m_Attributes; }

size_t model_data::vertex_count() const { return m_VertexCount; }

