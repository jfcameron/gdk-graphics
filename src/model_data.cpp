// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/exception.h>
#include <gdk/graphics/model_data.h>

#include <limits>
#include <optional>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace gdk;
using namespace gdk::graphics;

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
    
    if (result == m_Attributes.end()) throw exception("attribute not present");

    return result->second;
}

void model_data::overwrite(const size_t index, const model_data &otherVertexData) {
    for (const auto &[otherAttributeName, otherAttributeData] : otherVertexData.attributes()) {
        auto &thisAttributeData = get_attribute_data(otherAttributeName);
        thisAttributeData.overwrite(index, otherAttributeData);
    }
}

const attribute_data &model_data::get_attribute_data(const std::string &aAttributeName) const {
    auto result(m_Attributes.find(aAttributeName));

    if (result == m_Attributes.end()) throw exception("attribute not present");

    return result->second;
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
    
    const auto describes_the_same_attributes_as = [](const attribute_collection_type &aLeft,
        const attribute_collection_type &aRight) -> std::optional<std::string> {
        for (const auto &[name, data] : aRight)
            if (!aLeft.contains(name)) return name;

        return {};
    };

    if (const auto missing = describes_the_same_attributes_as(m_Attributes, other.m_Attributes))
        throw exception(std::string("model_data::push_back: the appended data has attribute \"")
            .append(*missing)
            .append("\" and this data does not; appending would leave the result ragged"));

    if (const auto missing = describes_the_same_attributes_as(other.m_Attributes, m_Attributes))
        throw exception(std::string("model_data::push_back: this data has attribute \"")
            .append(*missing)
            .append("\" and the appended data does not; appending would leave the result ragged"));

    const auto offset = m_VertexCount;

    const bool indexedResult = indexed() || other.indexed();

    if (indexedResult && !indexed()) {
        m_Indexes.resize(m_VertexCount);

        for (size_t i = 0; i < m_VertexCount; ++i)
            m_Indexes[i] = static_cast<index_value_type>(i);
    }

    for (const auto &[OtherAttributeName, otherAttributeData] : other.attributes())
        m_Attributes[OtherAttributeName].push_back(otherAttributeData);

    m_VertexCount = m_Attributes.begin()->second.components().size() / 
        m_Attributes.begin()->second.number_of_components_per_attribute();

    if (indexedResult) {
        const auto ceiling = std::size_t{std::numeric_limits<index_value_type>::max()} + 1;

        if (m_VertexCount > ceiling)
            throw exception(std::string("model_data::push_back: the result has ")
                .append(std::to_string(m_VertexCount))
                .append(" vertices, and an index is 16 bit, so it cannot name past ")
                .append(std::to_string(ceiling))
                .append(". Split the geometry across models."));

        std::vector<index_value_type> implied;

        if (!other.indexed()) {
            implied.resize(other.m_VertexCount);

            for (size_t i = 0; i < other.m_VertexCount; ++i)
                implied[i] = static_cast<index_value_type>(i);
        }

        const std::vector<index_value_type> &appended = other.indexed() ? other.m_Indexes : implied;

        m_Indexes.reserve(m_Indexes.size() + appended.size());

        for (const auto index : appended)
            m_Indexes.push_back(static_cast<index_value_type>(index + offset));
    }
}

model_data::model_data(attribute_collection_type &&aAttributeData,
    std::vector<index_value_type> &&aIndexes)
: model_data(std::move(aAttributeData)) {
    set_indexes(std::move(aIndexes));
}

model_data::model_data(attribute_collection_type &&aAttributeData) {
    const size_t numberOfUniformsFoundInFirstEntry(aAttributeData.begin()->second.number_of_attributes_in_component_data());
    for (const auto &[name, data] : aAttributeData) 
        if (data.number_of_attributes_in_component_data() != numberOfUniformsFoundInFirstEntry) 
            throw exception("the uniform data found in a vertex data's uniform collection must "
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
    return m_Indexes;
}

bool model_data::indexed() const {
    return !m_Indexes.empty();
}

void model_data::set_indexes(std::vector<model_data::index_value_type> aIndexes) {
    for (const auto index : aIndexes)
        if (index >= m_VertexCount)
            throw exception(std::string("model_data::set_indexes: index ")
                .append(std::to_string(index))
                .append(" names a vertex past the end of ")
                .append(std::to_string(m_VertexCount))
                .append(". Out of range indices are undefined behaviour in gl and fail silently "
                    "there, so they are refused here."));

    m_Indexes = std::move(aIndexes);
}

struct triangle {
    size_t index;
    vector3_type centroid;
};

static void sort_indexed_by_triangle(
    std::function<bool(triangle, triangle)> aSorter,
    model_data &aVertexData,
    const std::string &aPositionAttributeName) {
    const auto &attributes = aVertexData.attributes();

    const auto search = attributes.find(aPositionAttributeName);

    if (search == attributes.end()) return;

    const auto &positions = search->second.components();

    const auto &indexes = aVertexData.indexes();

    std::vector<triangle> triangles;

    triangles.reserve(indexes.size() / 3);

    for (size_t i = 0; i + 2 < indexes.size(); i += 3) {
        const auto centroid_component = [&](const size_t aComponent) {
            return (positions[indexes[i + 0] * 3 + aComponent]
                + positions[indexes[i + 1] * 3 + aComponent]
                + positions[indexes[i + 2] * 3 + aComponent]) / component_type(3.f);
        };

        triangles.push_back({i, {centroid_component(0), centroid_component(1),
            centroid_component(2)}});
    }

    std::sort(triangles.begin(), triangles.end(), aSorter);

    std::vector<model_data::index_value_type> sorted;

    sorted.reserve(indexes.size());

    for (const auto &each : triangles)
        for (size_t offset = 0; offset < 3; ++offset) sorted.push_back(indexes[each.index + offset]);

    for (size_t i = triangles.size() * 3; i < indexes.size(); ++i) sorted.push_back(indexes[i]);

    aVertexData.set_indexes(std::move(sorted));
}

static void sort_by_triangle(
    std::function<bool(triangle, triangle)> aSorter,
    model_data &aVertexData,
    const std::string &aPositionAttributeName) {
    if (aVertexData.indexed()) {
        sort_indexed_by_triangle(aSorter, aVertexData, aPositionAttributeName);

        return;
    }

    auto m_Attributes = aVertexData.attributes();

    if (auto search = m_Attributes.find(aPositionAttributeName); search != m_Attributes.end()) {
        auto &position_attribute_data = search->second.components();

        std::vector<triangle> triangles;

        size_t i(0);
        for (auto p = position_attribute_data.begin();
            position_attribute_data.end() - p >= 9; p += 9) {
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
       
            for (auto triangle : triangles) {
                auto pSortedTriangle = currentAttributeData.begin() + (triangle.index * currentComponentCount);

                for (size_t componentIndex = 0; componentIndex < 3 * currentComponentCount; ++componentIndex) {
                    auto newComponentPtr = pSortedTriangle + componentIndex;

                    new_attribute_data.push_back(*newComponentPtr);
                }

            }

            for (size_t component = triangles.size() * 3 * currentComponentCount;
                component < currentAttributeData.size(); ++component)
                new_attribute_data.push_back(currentAttributeData[component]);

            newAttributes[key] = {
                std::move(new_attribute_data),
                currentComponentCount
            };
        }

        aVertexData = model_data(std::move(newAttributes));
    }
}

void model_data::sort_by_nearest_triangle(
    const vector3_type &aObserverWorldPosition,
    matrix4x4_type aEntityInstanceWorldMatrix,
    const std::string &aObserverWorldPositionAttributeName) {
    aEntityInstanceWorldMatrix.inverse_affine(); 

    matrix4x4_type observerWorldMatrix;
    observerWorldMatrix.set_translation(aObserverWorldPosition);
    
    matrix4x4_type localObserver = aEntityInstanceWorldMatrix * observerWorldMatrix;
    vector3_type observerLocalPostion = localObserver.translation(); 

    sort_by_triangle(
        [&observerLocalPostion](triangle a, triangle b) {
            const auto cameraPos = observerLocalPostion;
            const auto entityPosA = a.centroid;
            const auto entityPosB = b.centroid;

            const auto aDist = cameraPos.distance_from(entityPosA);
            const auto bDist = cameraPos.distance_from(entityPosB);

            return (aDist < bDist);
        },
        *this,
        aObserverWorldPositionAttributeName);
}

void model_data::sort_by_furthest_triangle(
    const vector3_type &aObserverWorldPosition,
    matrix4x4_type aEntityInstanceWorldMatrix,
    const std::string &aObserverWorldPositionAttributeName) {
    aEntityInstanceWorldMatrix.inverse_affine(); 

    matrix4x4_type observerWorldMatrix;
    observerWorldMatrix.set_translation(aObserverWorldPosition);
    
    matrix4x4_type localObserver = aEntityInstanceWorldMatrix * observerWorldMatrix;
    vector3_type observerLocalPostion = localObserver.translation();

    sort_by_triangle(
        [&observerLocalPostion](triangle a, triangle b) {
            const auto cameraPos = observerLocalPostion;
            const auto entityPosA = a.centroid;
            const auto entityPosB = b.centroid;

            const auto aDist = cameraPos.distance_from(entityPosA);
            const auto bDist = cameraPos.distance_from(entityPosB);

            return (aDist > bDist);
        },
        *this,
        aObserverWorldPositionAttributeName);
}

void model_data::transform(const std::string &aPositionAttributeName, matrix4x4_type &aTransform) {
    auto &attributeData = get_attribute_data(aPositionAttributeName);
    if (attributeData.number_of_components_per_attribute() != COMPONENTS_PER_3D_ATTRIBUTE) 
        throw exception(std::string("model_data::transform expected 3 components per uniform: ") + aPositionAttributeName);

    auto &components = attributeData.components();

    for (auto p = components.begin(); p != components.end(); p += COMPONENTS_PER_3D_ATTRIBUTE) {
        auto x(p + 0), y(p + 1), z(p + 2);

        const auto transformed = aTransform * vector3_type(*x, *y, *z);

        *x = transformed.x;
        *y = transformed.y;
        *z = transformed.z;
    }
}

void model_data::transform(const std::string &aPositionAttributeName,
    const vector3_type &aPos,
    const quaternion_type &aRot,
    const vector3_type &aScale) {

    matrix4x4_type mat;
    mat.set_rotation_and_scale(aRot, aScale);
    mat.set_translation(aPos);
    transform(aPositionAttributeName, mat);
}

void model_data::transform(const std::string &a2DAttributeName,
    const vector2_type &aPos, const float aRotation,
    const vector2_type &aScale) {
    auto &attributeData = get_attribute_data(a2DAttributeName);
    if (attributeData.number_of_components_per_attribute() != COMPONENTS_PER_2D_ATTRIBUTE) 
        throw exception("model_data::transform expected 2 components per uniform");

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

