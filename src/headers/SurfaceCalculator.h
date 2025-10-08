//
// Created by Wiktor on 08.10.2025.
//

#ifndef SURFACECALCULATOR_H
#define SURFACECALCULATOR_H
#include <vector>
#include <cstdint>
#include "new_models.h"
#include <set>

class SurfaceCalculator {

   std::vector<std::vector<uint16_t>>* lines;
   std::set<NewModels::vec2>* vertices;
public:
   SurfaceCalculator(std::vector<std::vector<uint16_t>>* lines, std::set<NewModels::vec2>* vertices);

   std::vector<std::vector<uint16_t>> Calculate();

private:
   double CalculateArea(const std::vector<uint16_t> &polygon);

   bool PointInPolygon(const NewModels::vec2 &point, const std::vector<uint16_t> &polygon);

   bool IsPointInTriangle(const NewModels::vec2 &p, const NewModels::vec2 &a, const NewModels::vec2 &b,
                          const NewModels::vec2 &c);

   bool IsEar(const std::vector<uint16_t> &polygon, size_t i, const std::vector<NewModels::vec2> &vertex_cache);

   std::vector<std::vector<uint16_t>> TriangulatePolygon(const std::vector<uint16_t> &polygon);

   std::vector<std::vector<uint16_t>>& getLines();
};

#endif //SURFACECALCULATOR_H
