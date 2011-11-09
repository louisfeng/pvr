//----------------------------------------------------------------------------//

/*! \file PyPvr.cpp
  Contains the interface definition for the 'pvr' python module
 */

//----------------------------------------------------------------------------//
// Includes
//----------------------------------------------------------------------------//

// System includes

#include <boost/python.hpp>
#include <boost/python/reference_existing_object.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/object.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

// Library includes

#include <Field3D/InitIO.h>

#include <pvr/Exception.h>
#include <pvr/Globals.h>
#include <pvr/Interrupt.h>
#include <pvr/Log.h>
#include <pvr/Strings.h>
#include <pvr/Time.h>
#include <pvr/Types.h>

//----------------------------------------------------------------------------//
// Namespaces
//----------------------------------------------------------------------------//

using namespace std;
using namespace pvr;
using namespace pvr::Util;

//----------------------------------------------------------------------------//
// Forward declarations
//----------------------------------------------------------------------------//

void exportAttrTable();
void exportCameraFunctions();
void exportClassFactory();
void exportCurve();
void exportGeometry();
void exportGlobals();
void exportField3D();
void exportImage();
void exportLights();
void exportLog();
void exportModeler();
void exportModelerInput();
void exportNoiseFunctions();
void exportNoiseClasses();
void exportOccluders();
void exportParticles();
void exportPerspectiveCamera();
void exportPolygons();
void exportPrimitive();
void exportRaymarchers();
void exportRaymarchSamplers();
void exportRenderer();
void exportTransmittanceMap();
void exportVolumes();

//----------------------------------------------------------------------------//
// Exceptions
//----------------------------------------------------------------------------/

DECLARE_PVR_RT_EXC(Vec3AssignException, "Error assigning values to Vec3:");

//----------------------------------------------------------------------------//
// Helper classes
//----------------------------------------------------------------------------//

#if 0
template <class Vec3_T>
struct BindVec3
{
  static void 
}
#endif

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

template <typename T>
void vec_assign(std::vector<T>& v, boost::python::object o) 
{
  // Turn a Python sequence into an STL input range
  boost::python::stl_input_iterator<T> begin(o), end;
  v.assign(begin, end);
}

//----------------------------------------------------------------------------//

template <typename T>
void Vec2_assign(T &v, boost::python::object o) 
{
  // Turn a Python sequence into an STL input range
  boost::python::stl_input_iterator<typename T::BaseType> begin(o), end;
  if (begin == end) {
    throw Vec3AssignException("No elements in list");
  }
  v.x = *begin++;
  if (begin == end) {
    throw Vec3AssignException("Only one element in list");
  }
  v.y = *begin;
}

//----------------------------------------------------------------------------//

template <typename T>
void Vec3_assign(T &v, boost::python::object o) 
{
  // Turn a Python sequence into an STL input range
  boost::python::stl_input_iterator<typename T::BaseType> begin(o), end;
  if (begin == end) {
    throw Vec3AssignException("No elements in list");
  }
  v.x = *begin++;
  if (begin == end) {
    throw Vec3AssignException("Only one element in list");
  }
  v.y = *begin++;
  if (begin == end) {
    throw Vec3AssignException("Only two elements in list");
  }
  v.z = *begin;
}

//----------------------------------------------------------------------------//

void Quat_assign(Quat &q, boost::python::object o) 
{
  // Turn a Python sequence into an STL input range
  boost::python::stl_input_iterator<double> begin(o), end;
  if (begin == end) {
    throw Vec3AssignException("No elements in list");
  }
  q.r = *begin++;
  if (begin == end) {
    throw Vec3AssignException("Only one element in list");
  }
  q.v[0] = *begin++;
  if (begin == end) {
    throw Vec3AssignException("Only two elements in list");
  }
  q.v[1] = *begin++;
  if (begin == end) {
    throw Vec3AssignException("Only two elements in list");
  }
  q.v[2] = *begin;
}

//----------------------------------------------------------------------------//

template <typename T>
std::string Vec2_str(const T &self)
{
  // return "(" + str(self.x) + ", " + str(self.y) + ")";
  return str(self);
}

//----------------------------------------------------------------------------//

template <typename T>
std::string Vec3_str(const T &self)
{
  // return "(" + str(self.x) + ", " + str(self.y) + ", " + str(self.z) + ")";
  return str(self);
}

//----------------------------------------------------------------------------//

std::string Quat_str(const Quat &self)
{
  return "(" + str(self.r) + ", " + str(self.v[0]) + ", " + 
    str(self.v[1]) + ", " + str(self.v[2]) + ")";
}

//----------------------------------------------------------------------------//

std::string BBox_str(const BBox &self)
{
  return "[ min: " + Vec3_str(self.min) + ", max: " + Vec3_str(self.max) + " ]";
}

//----------------------------------------------------------------------------//
// PythonInterrupt
//----------------------------------------------------------------------------//

class PythonInterrupt : public pvr::Sys::Interrupt
{
public:
  static Ptr create()
  { return Ptr(new PythonInterrupt); }
  virtual bool abort() const
  {
    if (PyErr_CheckSignals() != 0) {
      Log::print("PVR got interrupt signal. Aborting.");
      return true;
    } else {
      return false;
    }
  }
};

//----------------------------------------------------------------------------//
// Initialization helper
//----------------------------------------------------------------------------//

void initPyPvr()
{
  // PVR globals
  Sys::Globals::init();
  // Field3D initialization
  Field3D::initIO();
  // Register interrupt handler
  pvr::Sys::Interrupt::setGlobalInterrupt(PythonInterrupt::create());
}

//----------------------------------------------------------------------------//
// Pvr python module
//----------------------------------------------------------------------------//

BOOST_PYTHON_MODULE(_pvr)
{
  using namespace boost::python;

  initPyPvr();

  // Imath::V2i ---

  class_<Imath::V2i>("V2i")
    .def(init<int>())
    .def(init<int, int>())
    .def("__str__",     &Vec2_str<Imath::V2i>)
    .def("assign",      &Vec2_assign<Imath::V2i>)
    .def_readwrite("x", &Imath::V2i::x)
    .def_readwrite("y", &Imath::V2i::y)
    ;  

  // Imath::V3d ---

  class_<Vector>("Vector")
    .def(init<double>())
    .def(init<double, double, double>())
    .def("__str__",     &Vec3_str<Vector>)
    .def("assign",      &Vec3_assign<Vector>)
    .def_readwrite("x", &Vector::x)
    .def_readwrite("y", &Vector::y)
    .def_readwrite("z", &Vector::z)
    ;
  class_<Imath::V3f>("V3f")
    .def(init<float>())
    .def(init<float, float, float>())
    .def("__str__",     &Vec3_str<Imath::V3f>)
    .def("assign",      &Vec3_assign<Imath::V3f>)
    .def("normalized",  &Imath::V3f::normalized)
    .def("dot",         &Imath::V3f::dot)
    .def("cross",       &Imath::V3f::cross)
    .def(self + self)
    .def(self - self)
    .def(self * self)
    .def(self * long())
    .def(self * float())
    .def_readwrite("x", &Imath::V3f::x)
    .def_readwrite("y", &Imath::V3f::y)
    .def_readwrite("z", &Imath::V3f::z)
    ;

  implicitly_convertible<Imath::V3f, Vector>();
  implicitly_convertible<Vector, Imath::V3f>();

  // Imath::V3i ---

  class_<Imath::V3i>("V3i")
    .def(init<int>())
    .def(init<int, int, int>())
    .def("__str__",     &Vec3_str<Imath::V3i>)
    .def("assign",      &Vec3_assign<Imath::V3i>)
    .def_readwrite("x", &Imath::V3i::x)
    .def_readwrite("y", &Imath::V3i::y)
    .def_readwrite("z", &Imath::V3i::z)
    ;

  // Imath::C3f ---

  class_<Color>("Color")
    .def(init<float>())
    .def(init<float, float, float>())
    .def("__str__",     &Vec3_str<Color>)
    .def("assign",      &Vec3_assign<Color>)
    .def_readwrite("r", &Color::x)
    .def_readwrite("g", &Color::y)
    .def_readwrite("b", &Color::z)
    ;

  // Euler ---

  class_<Euler>("Euler")
    .def(init<double, double, double>())
    .def(init<Vector>())
    .def("toQuat", &Euler::toQuat)
    ;

  // Quat ---

  class_<Quat>("Quat")
    .def(init<double, double, double, double>())
    .def(init<Quat>())
    .def("__str__", &Quat_str)
    .def("assign",  &Quat_assign)
    ;  

  // BBox ---

  class_<BBox>("BBox")
    .def(init<Vector>())
    .def(init<Vector, Vector>())
    .def("__str__",       &BBox_str)
    .def_readwrite("min", &BBox::min)
    .def_readwrite("max", &BBox::max)
    ;

  // std::vector ---

  class_<vector<int> >("IntVec")
    .def(vector_indexing_suite<std::vector<int> >())
    .def("assign",   &vec_assign<int>)
    .def("__iter__", boost::python::iterator<vector<int> >())
    ;

  class_<vector<float> >("FloatVec")
    .def(vector_indexing_suite<std::vector<float> >())
    .def("assign",   &vec_assign<float>)
    .def("__iter__", boost::python::iterator<vector<float> >())
    ;

  class_<vector<Imath::V3f> >("VectorVec")
    .def(vector_indexing_suite<std::vector<Imath::V3f> >())
    .def("assign",   &vec_assign<Imath::V3f>)
    // .def("__iter__", boost::python::iterator<vector<Imath::V3f> >())
    ;

  class_<vector<size_t> >("IdxVec")
    .def(vector_indexing_suite<std::vector<size_t> >())
    .def("assign",   &vec_assign<size_t>)
    .def("__iter__", boost::python::iterator<vector<size_t> >())
    ;

  class_<vector<string> >("StringVec")
    .def(vector_indexing_suite<std::vector<string> >())
    .def("assign",   &vec_assign<string>)
    .def("__iter__", boost::python::iterator<vector<string> >())
    ;

  // External definitions ---

  exportAttrTable();
  exportCameraFunctions();
  exportClassFactory();
  exportCurve();
  exportField3D();
  exportGeometry();
  exportGlobals();
  exportImage();
  exportLights();
  exportLog();
  exportModeler();
  exportModelerInput();
  exportNoiseFunctions();
  exportNoiseClasses();
  exportOccluders();
  exportParticles();
  exportPerspectiveCamera();
  exportPolygons();
  exportPrimitive();
  exportRaymarchers();
  exportRaymarchSamplers();
  exportRenderer();
  exportTransmittanceMap();
  exportVolumes();
}

//----------------------------------------------------------------------------//