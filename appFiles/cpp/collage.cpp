#include "collage.h"

#include <math.h>

double dataPenalty(int alpha, int label, Data *D, int i, int j, INSIDE_MODE insideMode);
double interactionPenalty(int pi, int pj, int qi, int qj, Data *D, int labelP, int labelQ, OUTSIDE_MODE outsideMode);
void computeGraph(Graph<double, double, double> &G, INSIDE_MODE insideMode, OUTSIDE_MODE outsideMode, const Mat &ResultLabel, Data *D, int alpha);


Collage::Collage(Data* Dat) {
    Collage::D = Dat;
}

//alpha_expansion
Mat Collage::computePhotomontage(INSIDE_MODE insideMode, OUTSIDE_MODE outsideMode) {

    /* Labels to be put on result image */
    Mat resultLabel(D->height, D->width, CV_8UC1);
    for (int i = 0; i < D->height; i++) {
        for (int j = 0; j < D->width; j++) {
            resultLabel.at<uchar>(i, j) = 0;
        }
    }

    int minCut = INT_MAX;
    bool improvment, skipZero = true;
    /* Alpha expansion */
    do {
        improvment = false;
        for (int alpha = 0; alpha < D->sources.size(); alpha++) {
            if (skipZero && alpha == 0) {
                skipZero = false;
                continue;
            }

            Graph<double, double, double> G(
                3 * D->height * D->width - D->width - D->height,
                2 * D->height * D->width - D->width - D->height);
            computeGraph(G, insideMode, outsideMode, resultLabel, D, alpha);
            int flow = G.maxflow();
            if (flow < minCut) {
                improvment = true;
                minCut = flow;
            }

            for (int i = 0; i < D->height; i++) {
                for (int j = 0; j < D->width; j++) {
                    if (G.what_segment(D->width * i + j) == Graph<double, double, double>::SINK) {
                            resultLabel.at<uchar>(i, j) = alpha;
                    }
                }
            }
        }

    } while (improvment);

    /* Compute result image */
    Mat resultImage(D->height, D->width,  CV_8UC3);

    for (int i = 0; i < D->width; i++) {
        for (int j = 0; j < D->height; j++) {
            for (int k = 0; k < D->sources.size(); k++) {
                Vec2d offset = D->offsets[k];

                if (resultLabel.at<uchar>(j, i) == k) {

                    unsigned char * p = (resultImage).ptr(j, i); // Y first, X after
                    unsigned char * pp = (D->sources[k]).ptr(j, i); // Y first, X after

                    p[0] = pp[0];   // B
                    p[1] = pp[1];   // G
                    p[2] = pp[2]; // R


                    if (D->SourceConstraints.at<uchar>(j, i) == 255) {
                        unsigned char * p = (D->Draw).ptr(j, i); // Y first, X after
                        unsigned char * pp = (D->sources[k]).ptr(j, i); // Y first, X after

                        p[0] = (pp[0] + D->colors[k % 7] [0]) / 2;   // B
                        p[1] = (pp[1] + D->colors[k % 7] [1]) / 2;   // G
                        p[2] = (pp[2] + D->colors[k % 7] [2]) / 2; // R


                    } else {
                        unsigned char * p = (D->Draw).ptr(j, i); // Y first, X after
                        p[0] = ( D->colors[k % 7] [0]);   // B
                        p[1] = ( D->colors[k % 7] [1]);   // G
                        p[2] = ( D->colors[k % 7] [2]); // R

                    }
                    break;
                }
            }
        }
    }

    return  resultImage;
}

/* calculate the penalty for a pixel */
double dataPenalty(int alpha, int label, Data *D, int i, int j, INSIDE_MODE insideMode) {
  double imageConstraint = D->SourceConstraints.at<uchar>(i, j);
  switch(insideMode) {
      case DESIGNATED_SOURCE:
          if (label != alpha) {
              if (imageConstraint == 255 || imageConstraint == label) return 0;
              } else {
                  if (imageConstraint == alpha || imageConstraint == 255) return 0;
              }
          return INT_MAX;
  }
}

/* calculates the interaction penalty between two pixels P and Q */
double interactionPenalty(int pi, int pj, int qi, int qj, Data *D, int labelP, int labelQ, OUTSIDE_MODE outsideMode) {
    Vec2d offsetP = D->offsets[labelP];
    Vec2d offsetQ = D->offsets[labelQ];
    Mat SLP = D->sources[labelP];
    Mat SLQ = D->sources[labelQ];
    Mat GXLP = D->gradientXSources[labelP];
    Mat GYLP = D->gradientYSources[labelP];
    Mat GXLQ = D->gradientXSources[labelQ];
    Mat GYLQ = D->gradientYSources[labelQ];
    double termP, termQ, termPx, termPy, termQx, termQy;
    if (!Collage::isInImage(pi, pj, offsetQ[0], offsetQ[1], SLQ) || !Collage::isInImage(qi, qj, offsetP[0], offsetP[1], SLP) ||
      !Collage::isInImage(pi, pj, offsetP[0], offsetP[1], SLP) || !Collage::isInImage(qi, qj, offsetQ[0], offsetQ[1], SLQ)) {
      return 0;
    }

    unsigned char * SLP_p = (SLP).ptr(pi-offsetP[1], pj-offsetP[0]); // Y first, X after
    unsigned char * SLQ_p = (SLQ).ptr(pi - offsetQ[1], pj - offsetQ[0]); // Y first, X after

    unsigned char * SLP_q = (SLP).ptr(qi-offsetP[1], qj-offsetP[0]); // Y first, X after
    unsigned char * SLQ_q = (SLQ).ptr(qi - offsetQ[1], qj - offsetQ[0]); // Y first, X after

    unsigned char * GXLP_p = (GXLP).ptr(pi - offsetP[1], pj - offsetP[0]); // Y first, X after
    unsigned char * GXLQ_p = (GXLQ).ptr(pi - offsetQ[1], pj - offsetQ[0]); // Y first, X after

    unsigned char * GYLP_p = (GYLP).ptr(pi - offsetP[1], pj - offsetP[0]); // Y first, X after
    unsigned char * GYLQ_p = (GYLP).ptr(pi - offsetQ[1], pj - offsetQ[0]); // Y first, X after


    unsigned char * GXLP_q = (GXLP).ptr(qi - offsetP[1], qj - offsetP[0]); // Y first, X after
    unsigned char * GXLQ_q = (GXLQ).ptr(qi - offsetQ[1], qj - offsetQ[0]); // Y first, X after


    unsigned char * GYLP_q = (GYLP).ptr(qi - offsetP[1], qj - offsetP[0]); // Y first, X after
    unsigned char * GYLQ_q = (GYLP).ptr(qi - offsetQ[1], qj - offsetQ[0]); // Y first, X after


    Vec3d sub_p;
    sub_p[0] = SLP_p[0] - SLQ_p[0];
    sub_p[1] = SLP_p[1] - SLQ_p[1];
    sub_p[2] = SLP_p[2] - SLQ_p[2];

    termP = norm(sub_p);

    Vec3d sub_q;
    sub_q[0] = SLP_q[0] - SLQ_q[0];
    sub_q[1] = SLP_q[1] - SLQ_q[1];
    sub_q[2] = SLP_q[2] - SLQ_q[2];

    termQ = norm(sub_q);

    Vec3d sub_GXLP;
    sub_GXLP[0] = GXLP_p[0] - GXLQ_p[0];
    sub_GXLP[1] = GXLP_p[1] - GXLQ_p[1];
    sub_GXLP[2] = GXLP_p[2] - GXLQ_p[2];
    termPx = norm(sub_GXLP);


    Vec3d sub_GYLP;
    sub_GYLP[0] = GYLP_p[0] - GYLQ_p[0];
    sub_GYLP[1] = GYLP_p[1] - GYLQ_p[1];
    sub_GYLP[2] = GYLP_p[2] - GYLQ_p[2];
    termPy = norm(sub_GYLP);


    Vec3d sub_GXLP_Q;
    sub_GXLP_Q[0] = GXLP_q[0] - GXLQ_q[0];
    sub_GXLP_Q[1] = GXLP_q[1] - GXLQ_q[1];
    sub_GXLP_Q[2] = GXLP_q[2] - GXLQ_q[2];
    termQx = norm(sub_GXLP_Q);


    Vec3d sub_GYLP_Q;
    sub_GYLP_Q[0] = GYLP_q[0] - GYLQ_q[0];
    sub_GYLP_Q[1] = GYLP_q[1] - GYLQ_q[1];
    sub_GYLP_Q[2] = GYLP_q[2] - GYLQ_q[2];
    termQy = norm(sub_GYLP_Q);
    return termP + termQ + sqrt(termPx * termPx + termPy * termPy) +
           sqrt(termQx * termQx + termQy * termQy);
}

/*    Create the flow graph for an image according to the modes */
void computeGraph(Graph<double, double, double> &G, INSIDE_MODE insideMode, OUTSIDE_MODE outsideMode, const Mat &ResultLabel, Data *D, int alpha) {
    G.add_node(D->height * D->width);
    int middleNode = D->height * D->width;

    for (int i = 0; i < D->height; i++) {
        for (int j = 0; j < D->width; j++) {
            double currentImage = ResultLabel.at<uchar>(i, j);
            double capacityToPuits = dataPenalty(alpha, currentImage, D, i, j, insideMode);
            double capacityToSource = INT_MAX;
            if (i - D->offsets[alpha][0] >= 0 && j - D->offsets[alpha][1] >= 0) {
                capacityToSource = dataPenalty(alpha, alpha, D, i, j, insideMode);
            }

            G.add_tweights(D->width * i + j, capacityToSource, capacityToPuits);

            if (i < D->height - 1) {
                int currVoisin = ResultLabel.at<uchar>(i + 1, j);
                G.add_node(1);
                capacityToPuits = interactionPenalty(i, j, i + 1, j, D, currentImage, currVoisin, outsideMode);
                G.add_tweights(middleNode, 0, capacityToPuits);
                double capacityToP = interactionPenalty(i, j, i + 1, j, D, currentImage, alpha, outsideMode);
                double capacityToQ = interactionPenalty(i, j, i + 1, j, D, alpha, currVoisin, outsideMode);
                G.add_edge(D->width * i + j, middleNode, capacityToP, capacityToP);
                G.add_edge(D->width * (i + 1) + j, middleNode, capacityToQ, capacityToQ);
                middleNode++;
            }

            if (j < D->width - 1) {
                int currVoisin = ResultLabel.at<uchar>(i, j + 1);
                G.add_node(1);
                capacityToPuits = interactionPenalty(i, j, i, j + 1, D, currentImage, currVoisin, outsideMode);
                if (currentImage != currVoisin) capacityToPuits++;
                G.add_tweights(middleNode, 0, capacityToPuits);
                double capacityToP = interactionPenalty(i, j, i, j + 1, D, currentImage, alpha, outsideMode);
                double capacityToQ = interactionPenalty(i, j, i, j + 1, D, alpha, currVoisin, outsideMode);
                G.add_edge(D->width * i + j, middleNode, capacityToP, capacityToP);
                G.add_edge(D->width * i + j + 1, middleNode, capacityToQ, capacityToQ);
                middleNode++;
            }
        }
    }
}
