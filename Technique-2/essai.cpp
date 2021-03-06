#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <X11/Xlib.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

using namespace std;
using namespace cv;

/* Variables globales */
String face_cascade_name = "haarcascade_frontalface_default.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
int centre = 0, moyenne_centre = 0, nombre_echantillon = 20;
int height = 0, width = 0;
int x = 0, y = 0;
int gauche = 0, droite = 0, haut = 0, bas = 0;

int calibration(int x, int oldx1, int oldx2, int newx1, int newx2)
{
        int diffnew = newx2 - newx1;
        int diffold = oldx2 - oldx1;
        int rapport = diffnew / diffold;
        x *= rapport;
        return x += newx1;
}

/** @function detectAndDisplay */
void detectAndDisplay(Mat frame, Display *dpy, Window root_window, int scr)
{
        Mat grImage;
        Mat resizeImage;
        resize(frame, resizeImage, Size(0, 0), 0.8, 0.8);

        int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
        //On met la couleur de la frame en noir et blanc pour faciliter son interprétation
        cvtColor(resizeImage, grImage, cv::COLOR_BGR2GRAY);
        //On crée un tableau de rectangle
        std::vector<Rect_<int>> faces;
        //On detecte le visage, chaque visage est stocké dans une case du tableau "faces"
        face_cascade.detectMultiScale(grImage, faces);
        //On boucle sur le nombre de visage sur la frame
        for (int i = 0; i < faces.size(); i++)
        {
                Rect face_i = faces[i];
                //Affiche un rectangle autour du visage pour montrer la détection du visage à y'utilisateur
                //Rectangle couleur verte
                rectangle(resizeImage, face_i, CV_RGB(0, 255, 0), 1);

                centre = ((face_i.size().height) / 2) + face_i.x;
                //Calcul de l'orientation de la tête haut et bas grâce aux échantillons
                //précédemment calculés
                if (centre - moyenne_centre > 0)
                {
                        haut = (moyenne_centre - centre);
                        bas = 0;
                        if (haut > 30 || haut < 5)
                                haut = 0;
                }
                else
                {
                        haut = 0;
                        bas = -(moyenne_centre - centre);
                        if (bas > 30 || bas < 5)
                                bas = 0;
                }

                Mat faceROI = grImage(faces[i]);

                std::vector<Rect> eyes;

                //Détecte les yeux dans chaque visage détecté
                eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, Size(30, 30));

                //On boucle sur le nombre de yeux sur la frame
                for (size_t j = 0; j < eyes.size(); j++)
                {
                        //on trouve le point central de y'oeil
                        Point center(faces[i].x + eyes[j].x + eyes[j].width * 0.5, faces[i].y + eyes[j].y + eyes[j].height * 0.5);
                        //cvRound arrondi à y'entier le plus proche
                        int radius = cvRound((eyes[j].width + eyes[j].height) * 0.25);
                        //on trace le cercle sur y'image "frame" de centre "center" et de rayon "radius"
                        circle(resizeImage, center, radius, Scalar(255, 0, 0), 4, 8, 0);

                        if (eyes[0].x < eyes[1].x)
                        {
                                x1 = center.x;
                                y1 = center.y;
                        }
                        else
                        {
                                x2 = center.x;
                                y2 = center.y;
                        }
                }

                if (y2 - y1 > 0)
                {
                        droite = (y2 - y1);
                        gauche = 0;
                        if (droite > 30 || droite < 5)
                                droite = 0;
                }
                else
                {
                        gauche = -(y2 - y1);
                        droite = 0;
                        if (gauche > 30 || gauche < 5)
                                gauche = 0;
                }

                printf("gauche: %d droite: %d haut: %d bas: %d\n", calibration(gauche, 5, 30, 5, width), calibration(droite, 5, 30, 5, width), calibration(haut, 5, 30, 5, height), calibration(bas, 5, 30, 5, height));

                x = width / 2;
                y = height / 2;
                x = x + (calibration(droite, 5, 30, 5, width / 2) - calibration(gauche, 5, 30, 5, width / 2));
                y = y - (calibration(haut, 5, 30, 5, height / 2) - calibration(bas, 5, 30, 5, height / 2));
                //mettre à jour le curseur
                XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, x, y);
                XFlush(dpy);
                usleep(50);
        }
        imshow("face", resizeImage);
}

int main(int argc, const char **argv)
{

        CommandLineParser parser(argc, argv,
                                 "{help h||}"
                                 "{face_cascade|data/haarcascades/haarcascade_frontalface_alt.xml|Path to face cascade.}"
                                 "{eyes_cascade|data/haarcascades/haarcascade_eye_tree_eyeglasses.xml|Path to eyes cascade.}"
                                 "{camera|0|Camera device number.}");
        parser.about("\nThis program demonstrates using the cv::CascadeClassifier class to detect objects (Face + eyes) in a video stream.\n"
                     "You can use Haar or LBP features.\n\n");
        parser.printMessage();

        //Choisir y'ecran
        Display *dpy = XOpenDisplay(0);
        int scr = XDefaultScreen(dpy);
        Window root_window = XRootWindow(dpy, scr);

        //Calculer la taille et de y'ecran
        height = DisplayHeight(dpy, scr);
        width = DisplayWidth(dpy, scr);
        std::cout << "Screen size : " << width << "x" << height << std::endl;

        //-- 1. Load the cascades
        if (!face_cascade.load(face_cascade_name))
        {
                cout << "--(!)Error loading face cascade\n";
                return -1;
        };
        if (!eyes_cascade.load(eyes_cascade_name))
        {
                cout << "--(!)Error loading eyes cascade\n";
                return -1;
        };
        int camera_device = parser.get<int>("camera");
        VideoCapture capture;
        //-- 2. Read the video stream
        capture.open(camera_device);
        if (!capture.isOpened())
        {
                cout << "--(!)Error opening video capture\n";
                return -1;
        }
        Mat frame;
        namedWindow("face", 1);
        Mat grImage;
        Mat resizeImage;
        /* On récupère un nombre d'échantillon de frame. Ces échantillons vont servir à "calibrer" le visage
        *  en lui donnant une position initiale avec laquelle on pourra comparer les futurs mouvement. Via
        *  cette comparaison on sera alors capable de dire si on oriente la tête ou pas.
        * */
        for (int i = 0; i < nombre_echantillon; i++)
        {
                Mat frame;
                capture >> frame; // get a new frame from camera
                resize(frame, resizeImage, Size(0, 0), 0.8, 0.8);

                //On met la couleur de la frame en noir et blanc pour faciliter son interprétation
                cvtColor(resizeImage, grImage, cv::COLOR_BGR2GRAY);
                //On crée un tableau de rectangle
                std::vector<Rect_<int>> faces;
                //On detecte le visage, chaque visage est stocké dans une case du tableau "faces"
                face_cascade.detectMultiScale(grImage, faces);

                //Récupère le premier visage du tableau
                Rect face_i = faces[0];
                //Affiche un rectangle autour du visage pour montrer la détection du visage à y'utilisateur
                //Rectangle couleur rouge
                rectangle(resizeImage, face_i, CV_RGB(255, 0, 0), 1);

                //Calcul du centre du rectangle
                centre = ((face_i.size().height) / 2) + face_i.x;
                moyenne_centre += centre;

                imshow("face", resizeImage);
                //Nombre de fps (frame per seconds)
                if (waitKey(30) >= 0)
                        break;
        }

        moyenne_centre = moyenne_centre / nombre_echantillon;

        //Initialiser la position du curseur au milieu de y'écran
        x = width / 2;
        y = height / 2;

        while (capture.read(frame))
        {
                if (frame.empty())
                {
                        cout << "--(!) No captured frame -- Break!\n";
                        break;
                }
                //-- 3. Apply the classifier to the frame
                detectAndDisplay(frame, dpy, root_window, scr);
                if (waitKey(10) == 27)
                {
                        break; // escape
                }
        }

        return 0;
}
