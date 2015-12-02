#ifndef SCALAR_H
#define SCALAR_H

#include <string>
#include <vector>

class Scalar {
public:
  explicit Scalar( std::string name );
  void store(float value);

  // GETTERS
  float min() { return _min; }
  float max() { return _max; }
  std::string name() { return _name; }
  int index() { return _index; }
  std::vector<std::string> &labels() { return _labels; }

  // SETTERS
  void setMin(float min) { _min = min; }
  void setMax(float max) { _max = max; }
  void setName(std::string name) { _name = name; }
  void setIndex(int index) { _index = index; }

private:
  float                    _min;
  float                    _max;
  std::string              _name;
  int                      _index;
  std::vector<std::string> _labels;
};

#endif // SCALAR_H
