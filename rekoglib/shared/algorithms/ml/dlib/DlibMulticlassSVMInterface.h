//
// DlibMulticlassSVMInterface.h
// Author: Michael Bao
// Date: 9/3/2015
//

#include "shared/algorithms/ml/dlib/DlibMLInterface.h"
#include "dlib/svm_threaded.h"
#include "dlib/matrix/matrix.h"
#include "dlib/serialize.h"

template<typename ImagePixelType = unsigned char,
    typename ClassificationFeatureType = dlib::matrix<double, 3, 1>,
    typename ClassificationResultType = int>
class DlibMulticlassSVMInterface: public DlibMLInterface<ImagePixelType>
{
public:
    virtual void InitializeInterface() override
    {
        multiclassSVM = std::make_unique<DlibSvmType>();
        assert(multiclassSVM);
    }

    virtual void SaveInterface(const std::string& filename)
    {
        if (!multiclassSVM) {
            REKOG_ERROR("Trying to save an invalid DlibMulticlassSVMInterface");
            return;
        }
        dlib::serialize(filename) << *multiclassSVM;
    }

    virtual void InitializeInterface(const std::string& filename)
    {
        InitializeInterface();
        dlib::deserialize(filename) >> *multiclassSVM;
    }

private:
    using DlibSvmType = dlib::svm_multiclass_linear_trainer<dlib::linear_kernel<ClassificationFeatureType>, ClassificationResultType>;
    std::unique_ptr<DlibSvmType> multiclassSVM;
};

