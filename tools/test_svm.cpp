#include "headers.h"
#include "svm.h"


template <int A, int B> // A is l, B  is num_elements (+l)
struct svm_model_data {

    svm_parameter params;

    svm_node node_data[B];

    double coef_data[A];
    double * p;

    svm_node* SV[B];		/* SVs (SV[l]) */
};

svm_model_data<1, 2> the_model = {{3,3,0,0.1}, {{0, 0.0}, {-1, 0}},{0.0}};

template <int A, int B>
void fill_model(svm_model_data<A,B> &model_data, svm_model * model) {
    model->param = model_data.params;
    model->l = A;
    model->nr_class = 2;
    model->SV = model_data.SV;
    int j = 0;
    for (int i = 0; i < A; ++i) {
        model_data.SV[i] = &model_data.node_data[j];
        while(model_data.node_data[j].index != -1) j++;
    }
    model_data.p = &model_data.coef_data[0];
    model->sv_coef = &model_data.p;

}

int main(int argc, const char *argv[]) {

    svm_model model;
    fill_model(the_model, &model);
//    the_model.fill_svm_model(&model);

    printf("%d\n", model.param.svm_type);



    printf("Hello world");

    return 0;
}
