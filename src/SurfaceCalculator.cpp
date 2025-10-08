#include "headers/SurfaceCalculator.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <set>
#include <iostream> // For debugging

SurfaceCalculator::SurfaceCalculator(std::vector<std::vector<uint16_t>>* lines, std::set<NewModels::vec2>* vertices) {
    if (!lines || !vertices) {
        throw std::invalid_argument("Null pointer for lines or vertices");
    }
    this->lines = lines;
    this->vertices = vertices;
}

double SurfaceCalculator::CalculateArea(const std::vector<uint16_t>& polygon) {
    if (polygon.size() < 3) {
        return 0.0;
    }
    double area = 0.0;
    for (size_t i = 0; i < polygon.size(); ++i) {
        auto it1 = vertices->begin();
        std::advance(it1, polygon[i]);
        auto p1 = *it1;
        auto it2 = vertices->begin();
        std::advance(it2, polygon[(i + 1) % polygon.size()]);
        auto p2 = *it2;
        area += static_cast<double>(p1.x) * p2.y - static_cast<double>(p2.x) * p1.y;
    }
    return area / 2.0; // Signed area
}

bool SurfaceCalculator::PointInPolygon(const NewModels::vec2& point, const std::vector<uint16_t>& polygon) {
    if (polygon.size() < 3) {
        return false;
    }
    size_t count = 0;
    auto it = vertices->begin();
    std::advance(it, polygon[0]);
    auto p1 = *it;

    for (size_t i = 1; i <= polygon.size(); ++i) {
        auto it2 = vertices->begin();
        std::advance(it2, polygon[i % polygon.size()]);
        auto p2 = *it2;

        if ((point.y > std::min(p1.y, p2.y)) &&
            (point.y <= std::max(p1.y, p2.y)) &&
            (point.x <= std::max(p1.x, p2.x))) {
            if (p1.y == p2.y) {
                if (point.x <= std::max(p1.x, p2.x)) {
                    ++count;
                }
            } else {
                double xIntersect = p1.x + (point.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
                if (p1.x == p2.x || point.x <= xIntersect) {
                    ++count;
                }
            }
        }
        p1 = p2;
    }
    return count % 2 == 1;
}

bool SurfaceCalculator::IsPointInTriangle(const NewModels::vec2& p, const NewModels::vec2& a, const NewModels::vec2& b, const NewModels::vec2& c) {
    auto sign = [](const NewModels::vec2& p1, const NewModels::vec2& p2, const NewModels::vec2& p3) {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    };
    double d1 = sign(p, a, b);
    double d2 = sign(p, b, c);
    double d3 = sign(p, c, a);
    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(has_neg && has_pos);
}

bool SurfaceCalculator::IsEar(const std::vector<uint16_t>& polygon, size_t i, const std::vector<NewModels::vec2>& vertex_cache) {
    if (polygon.size() < 3 || i >= polygon.size()) {
        return false;
    }

    size_t n = polygon.size();
    size_t prev = (i == 0) ? n - 1 : i - 1;
    size_t next = (i + 1) % n;

    auto a = vertex_cache[polygon[prev]];
    auto b = vertex_cache[polygon[i]];
    auto c = vertex_cache[polygon[next]];

    // Check if the angle at b is convex (positive for counterclockwise polygon)
    auto cross = [](const NewModels::vec2& p1, const NewModels::vec2& p2, const NewModels::vec2& p3) {
        double dx1 = static_cast<double>(p2.x) - p1.x;
        double dy1 = static_cast<double>(p2.y) - p1.y;
        double dx2 = static_cast<double>(p3.x) - p2.x;
        double dy2 = static_cast<double>(p3.y) - p2.y;
        return dx1 * dy2 - dy1 * dx2;
    };
    constexpr double epsilon = 1e-10;
    double cross_product = cross(a, b, c);
    if (cross_product <= epsilon) {
        std::cerr << "Vertex " << i << " rejected: non-convex (cross = " << cross_product << ")\n";
        return false;
    }

    // Check if any other vertex lies inside triangle (a, b, c)
    for (size_t j = 0; j < n; ++j) {
        if (j != prev && j != i && j != next) {
            auto p = vertex_cache[polygon[j]];
            if (IsPointInTriangle(p, a, b, c)) {
                std::cerr << "Vertex " << i << " rejected: point " << j << " inside triangle\n";
                return false;
            }
        }
    }
    return true;
}

std::vector<std::vector<uint16_t>> SurfaceCalculator::TriangulatePolygon(const std::vector<uint16_t>& polygon) {
    std::vector<std::vector<uint16_t>> triangles;
    if (polygon.size() < 3) {
        std::cerr << "TriangulatePolygon: polygon too small (" << polygon.size() << " vertices)\n";
        return triangles;
    }

    // Validate and cache vertices
    std::vector<NewModels::vec2> vertex_cache(polygon.size());
    for (size_t i = 0; i < polygon.size(); ++i) {
        if (polygon[i] >= vertices->size()) {
            std::cerr << "TriangulatePolygon: invalid vertex index " << polygon[i] << "\n";
            return triangles;
        }
        auto it = vertices->begin();
        std::advance(it, polygon[i]);
        vertex_cache[i] = *it;
    }

    std::vector<uint16_t> remaining = polygon;
    while (remaining.size() > 3) {
        bool found_ear = false;
        for (size_t i = 0; i < remaining.size(); ++i) {
            if (IsEar(remaining, i, vertex_cache)) {
                size_t n = remaining.size();
                size_t prev = (i == 0) ? n - 1 : i - 1;
                size_t next = (i + 1) % n;

                triangles.push_back({remaining[prev], remaining[i], remaining[next]});
                remaining.erase(remaining.begin() + i);
                found_ear = true;
                break;
            }
        }
        if (!found_ear) {
            std::cerr << "TriangulatePolygon: no ear found, remaining vertices: " << remaining.size() << "\n";
            return triangles;
        }
    }
    if (remaining.size() == 3) {
        triangles.push_back(remaining);
    }
    return triangles;
}

std::vector<std::vector<uint16_t>> SurfaceCalculator::Calculate() {
    std::vector<std::vector<uint16_t>> all_triangles;

    // Validate input
    for (const auto& poly : *lines) {
        for (const auto& idx : poly) {
            if (idx >= vertices->size()) {
                std::cerr << "Calculate: invalid vertex index " << idx << "\n";
                return all_triangles;
            }
        }
    }

    // Step 1: Calculate areas and sort by absolute area (largest to smallest)
    std::vector<std::pair<double, size_t>> sorted_areas(lines->size());
    for (size_t i = 0; i < lines->size(); ++i) {
        double area = CalculateArea((*lines)[i]);
        sorted_areas[i] = {std::abs(area), i};
    }
    std::sort(sorted_areas.begin(), sorted_areas.end()); // Fixed: largest to smallest

    // Step 2: Classify polygons as outer or holes
    std::vector<std::set<size_t>> contained_holes(lines->size());
    for (size_t i = 0; i < sorted_areas.size(); ++i) {
        size_t poly_idx = sorted_areas[i].second;
        std::vector<uint16_t>& polygon = (*lines)[poly_idx];
        bool is_hole = false;

        for (size_t j = 0; j < i; ++j) { // Check larger polygons
            size_t outer_idx = sorted_areas[j].second;
            if (outer_idx == poly_idx) continue;

            size_t count = 0;
            for (uint16_t v : polygon) {
                auto it = vertices->begin();
                std::advance(it, v);
                if (PointInPolygon(*it, (*lines)[outer_idx])) {
                    ++count;
                }
            }
            if (count == polygon.size()) {
                contained_holes[outer_idx].insert(poly_idx);
                is_hole = true;
                break;
            }
        }
    }

    // Step 3: Triangulate each outer polygon with its holes
    for (size_t i = 0; i < lines->size(); ++i) {
        if (contained_holes[i].empty() && CalculateArea((*lines)[i]) < 0) {
            continue; // Skip holes
        }

        // Merge outer polygon with its holes
        std::vector<uint16_t> merged_polygon = (*lines)[i];
        for (size_t hole_idx : contained_holes[i]) {
            std::vector<uint16_t>& hole = (*lines)[hole_idx];

            // Find bridge: rightmost vertex of hole to a visible vertex on outer polygon
            uint16_t max_x_idx = hole[0];
            auto it = vertices->begin();
            std::advance(it, max_x_idx);
            double max_x = it->x;
            size_t max_x_hole_idx = 0;
            for (size_t j = 1; j < hole.size(); ++j) {
                auto it2 = vertices->begin();
                std::advance(it2, hole[j]);
                if (it2->x > max_x) {
                    max_x = it2->x;
                    max_x_idx = hole[j];
                    max_x_hole_idx = j;
                }
            }

            // Simple visibility test: choose the closest outer vertex (improve if needed)
            uint16_t outer_vertex = merged_polygon[0];
            size_t outer_vertex_idx = 0;
            auto it_outer = vertices->begin();
            std::advance(it_outer, outer_vertex);
            double min_dist = std::hypot(it_outer->x - max_x, it_outer->y - it->y);
            for (size_t j = 1; j < merged_polygon.size(); ++j) {
                auto it2 = vertices->begin();
                std::advance(it2, merged_polygon[j]);
                double dist = std::hypot(it2->x - max_x, it2->y - it->y);
                if (dist < min_dist) {
                    min_dist = dist;
                    outer_vertex = merged_polygon[j];
                    outer_vertex_idx = j;
                }
            }

            // Merge: outer -> hole -> back to outer
            std::vector<uint16_t> new_merged;
            size_t j = outer_vertex_idx;
            do {
                new_merged.push_back(merged_polygon[j]);
                j = (j + 1) % merged_polygon.size();
            } while (j != outer_vertex_idx);
            new_merged.push_back(outer_vertex); // Bridge to hole
            new_merged.push_back(max_x_idx);

            for (size_t k = 0; k < hole.size(); ++k) {
                new_merged.push_back(hole[(max_x_hole_idx + k) % hole.size()]);
            }
            new_merged.push_back(max_x_idx); // Bridge back
            merged_polygon = new_merged;
        }

        // Ensure counterclockwise orientation
        if (CalculateArea(merged_polygon) < 0) {
            std::reverse(merged_polygon.begin(), merged_polygon.end());
        }

        // Triangulate the merged polygon
        auto triangles = TriangulatePolygon(merged_polygon);
        all_triangles.insert(all_triangles.end(), triangles.begin(), triangles.end());
    }

    return all_triangles;
}