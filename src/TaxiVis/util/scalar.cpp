#include "scalar.h"

Scalar::Scalar( std::string name ):
  _min( 0.f ), _max( 0.f ), _name( name ), _index( -1 ) {}

void Scalar::store( float value ) {
  if ( value > _max ) _max = value;
  if ( value < _min ) _min = value;
}
