#pragma once
#include "alignment_checker/AlignmentQuality.h"
#include "alignment_checker/ScanType.h"
#include "map"
#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/eigen.h>
namespace py = pybind11;
using namespace pybind11::literals; 
//!*
//! Binary classification only, can be used for multiple tasks
//!*/
namespace CorAlignment{
using std::cout; using std::cerr; using std::endl;


class AlignmentLearningInterface{

protected:
public:

  // PythonClassifierInterface(){}
  AlignmentLearningInterface();

  //!*
  //! Interface of binary classification
  //*

  void fit( const std::string& model);

  Eigen::MatrixXd predict_proba() {return predict_proba(X_);} // X_{n x m}. n rows samples, m quality measures. return y_pred_{n x 1}

  Eigen::MatrixXd predict_proba(const Eigen::MatrixXd& X); // X_{n x m}. n rows samples, m quality measures. return y_pred_{n x 1}

  Eigen::MatrixXd predict(const Eigen::MatrixXd& X, const Eigen::MatrixXd& y_pred);

  Eigen::MatrixXd predict(){return this->predict(this->X_, this->y_);} // Uses predict proba internally, rahter then reimplementing it

  void AddDataPoint(Eigen::MatrixXd X_i, Eigen::MatrixXd y_i); // extends X_ and y_ with an additional datapoint


  //! INPUT /OUTPUT
  //! \brief LoadData containing rows of [x_{i,:} y_i]
  //! \param path
  //!

  void LoadData(const std::string& path); //

  //!
  //! \brief SaveData
  //! \param path
  //!
  void SaveData(const std::string& path); // Containing rows of [x_{i,:} y_i]

  //! Members

  Eigen::MatrixXd X_; // training data
  Eigen::MatrixXd y_; // training labels

private:
  //Should hold a pybind object

  // pybind11
  static py::scoped_interpreter guard_;
  py::module sklearn_;
  py::object py_clf_;
};



// Encapsulates CorAl specific tasks


class ScanLearningInterface{
  public:

  typedef struct scan
  {
    Eigen::Affine3d T;
    pcl::PointCloud<pcl::PointXYZI>::Ptr cld;
    pcl::PointCloud<pcl::PointXYZI>::Ptr cldPeaks;
    CFEAR_Radarodometry::MapNormalPtr CFEAR;
  }s_scan;

  ScanLearningInterface() {}

  //!
  //! \brief AddTrainingData perform synthetic missalignment and adds positive and nevative training data
  //! \param T pose of current scan
  //! \param cloud Current Scan non-peaks point cloud
  //! \param cloud_peaks peaks in current point cloud
  //! \param CFEARScan CFEAR features
  //!
  void AddTrainingData(s_scan& current); // is this too similar to AlignmentQualityInterface, Reuse some of the code? Incremental interface, update for every frame, make sure to use and update prev_

  void PredAlignment(s_scan& current, s_scan& prev, std::map<std::string,double>& quality);

  // e.g. 2 text files of data. starts training

  void LoadData(const std::string& dir);

  //!
  //! \brief SaveData Stores dir/CFEAR.txt and dir/CorAL.txt
  //! \param dir
  //!
  void SaveData(const std::string& dir);

  private:

  AlignmentLearningInterface cfear_class, coral_class;
  s_scan prev_;
  unsigned int frame_ = 0;

};


}
