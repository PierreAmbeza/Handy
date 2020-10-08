#include <iostream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>



using namespace cv;
using namespace std;

int c = 0;//Compteur de défauts de convexité qui permet de déterminer le choix de l'utilisateur
int x, y, rect_width;
int low_H = 0, low_S = 0, low_V = 0, high_H = 180, high_S = 255, high_V = 255;//Valeurs de seuillage HSV
Rect roi;///Size and location of both roi
//Region d'intérêt dans laquelle le joueur doit placer sa main
int user_score = 0, computer_score = 0;//Compteurs des scores
Mat computer_image;


///Frame est l'image capturé par la caméra, frame_hsv est l'image convertie en HSV et HSV_threshold_output l'image HSV seuillée
Mat frame, frame_hsv, hsv_threshold_output;



void finger_counter(int, void *);//Déclaration de l'en tête de la fonction


///Fonctions permettant de garder la valeur du seuillage basse inférieur à celle du seuillage haut et vice versa

static void on_low_H_thresh_trackbar(int, void *)
{
    if(high_H != 0)
    {
        low_H = min(high_H-1, low_H);
        setTrackbarPos("Low H", "HSV Threshold", low_H);
    }
    else
    {
        low_H = high_H;
        setTrackbarPos("Low H", "HSV Threshold", low_H);
    }
}

static void on_high_H_thresh_trackbar(int, void *)
{
    if(low_H != 180)
    {
        high_H = max(high_H, low_H+1);
        setTrackbarPos("High H", "HSV Threshold", high_H);
    }
    else
    {
        high_H = low_H;
        setTrackbarPos("High H", "HSV Threshold", high_H);
    }
}

static void on_low_S_thresh_trackbar(int, void *)
{
    if(high_S != 0)
    {
        low_S = min(high_S-1, low_S);
        setTrackbarPos("Low S", "HSV Threshold", low_S);
    }
    else
    {
        low_S = high_S;
        setTrackbarPos("Low S", "HSV Threshold", low_S);
    }
}

static void on_high_S_thresh_trackbar(int, void *)
{
    if(low_S != 255)
    {
        high_S = max(low_S+1, high_S);
        setTrackbarPos("High S", "HSV Threshold", high_S);
    }
    else
    {
        high_S = low_S;
        setTrackbarPos("High S", "HSV Threshold", high_S);
    }
}

static void on_low_V_thresh_trackbar(int, void *)
{
    if(high_V != 0)
    {
        low_V = min(high_V-1, low_V);
        setTrackbarPos("Low V", "HSV Threshold", low_V);
    }
    else
    {
        low_V = high_V;
        setTrackbarPos("Low V", "HSV Threshold", low_V);
    }
}

static void on_high_V_thresh_trackbar(int, void *)
{
    if(low_V != 255)
    {
        high_V = max(low_V+1, high_V);
        setTrackbarPos("High V", "HSV Threshold", high_V);
    }
    else
    {
        high_V = low_V;
        setTrackbarPos("High V", "HSV Threshold", high_V);
    }
}


///Fonction qui initialise la partie en dessinant la zone où le joueur place sa main et affiche les scores
void game_initialization()
{
    rectangle(frame, Point(x, y), Point(x+rect_width, y+rect_width), Scalar(255, 0, 0), 5, 8, 0);
    putText(frame, "User:"+to_string(user_score), Point(frame.size().width/20, frame.size().height/10), FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 0), 2);
    putText(frame, "Computer:"+to_string(computer_score), Point(frame.size().width*3/4, frame.size().height/10), FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 0), 2);
}

void roi_initialization()
{
    x = frame.size().width*3/8;
    y = frame.size().height/4;
    rect_width = frame.size().width/4;
    roi = Rect(x, y, rect_width, rect_width);
}


///Fonction qui permet à l'ordinateur de choisir aléatoirement
string computer_choice_function()
{
    string Tab[] = {"Pierre", "Feuille", "Ciseaux"};
    int choice;
    choice = rand() % 3;
    cout << Tab[choice] << endl;
    return Tab[choice];
}

void image_loading(string computer_choice)
{
    if(computer_choice == "Pierre")
        computer_image = imread("Pierre.png");
    else if(computer_choice == "Ciseaux")
        computer_image = imread("Ciseaux.jpg");
    else
        computer_image = imread("Feuille.jpg");
}

///Détermination du vainqueur en fonction des choix de signe effectué
int winner(int user_choice)
{
    string computer_choice = computer_choice_function();
    image_loading(computer_choice);
    if(user_choice == 4 && computer_choice == "Pierre")//Si l'utilisateur choisi Feuille et l'ordinateur Pierre
        return 1;//Retourne 1 pour la victoire de l'utilisateur
    if(user_choice == 4 && computer_choice == "Feuille")//Si l'utilisateur choisi Feuille et l'ordinateur Feuille
        return 2;//Retourne 2 si égalité
    if(user_choice == 2 && computer_choice == "Feuille")//Si l'utilisateur choisi Ciseaux et l'ordinateur Feuille
        return 1;//Retourne 1 pour la victoire de l'utilisateur
    if(user_choice == 2 && computer_choice == "Ciseaux")
        return 2;//Retourne 2 si égalité
    if(user_choice == 0 && computer_choice == "Ciseaux")//Si l'utilisateur choisi Pierre et l'ordinateur Ciseaux
        return 1;//Retourne 1 pour la victoire de l'utilisateur
    if(user_choice == 0 && computer_choice == "Pierre")//Si l'utilisateur choisi Pierre et l'ordinateur Pierre
        return 2;//Retourne 2 si égalité
    return 0;
}

///Fonction d'affichage du résultat en fonction du vainqueur
void display_result(int user_choice)
{
    int result = winner(user_choice);
    if(result == 1)//Si l'utilisateur gagne, on affiche un rectangle vert
    {
        user_score++;
        rectangle(frame, Point(x, y), Point(x+rect_width, y+rect_width), Scalar(0, 255, 0), 5, 8, 0);
        imshow("Game", frame);
        imshow("Computer choice", computer_image);
        waitKey(500);
    }
    else if(result == 2)//S'il y a match nul, on affiche un rectangle jaune
    {
        rectangle(frame, Point(x, y), Point(x+rect_width, y+rect_width), Scalar(0, 255, 255), 5, 8, 0);
        imshow("Game", frame);
        imshow("Computer choice", computer_image);
        
        waitKey(500);
    }
    else//Si c'est l'ordinateur qui gagne, on affiche un rectangle rouge
    {
        computer_score++;
        rectangle(frame, Point(x, y), Point(x+rect_width, y+rect_width), Scalar(0, 0, 255), 5, 8, 0);
        imshow("Game", frame);
        imshow("Computer choice", computer_image);
        waitKey(500);
    }
}

int main(int argc, char* argv[])
{
    
    VideoCapture capture(0);
    if(!capture.isOpened())
    {
        cerr << "Unable to open camera" << endl;
        return 0;
    }
    
    namedWindow("HSV Threshold");
    namedWindow("Game");
    
    ///Creation des trackbars pour régler les bornes du seuillage
    createTrackbar("Low H", "HSV Threshold", &low_H, 180, on_high_H_thresh_trackbar);
    createTrackbar("Low S", "HSV Threshold", &low_S, 255, on_high_S_thresh_trackbar);
    createTrackbar("Low V", "HSV Threshold", &low_V, 255, on_high_V_thresh_trackbar);
    createTrackbar("High H", "HSV Threshold", &high_H, 180, on_low_H_thresh_trackbar);
    createTrackbar("High S","HSV Threshold", &high_S, 255, on_low_S_thresh_trackbar);
    createTrackbar("High V", "HSV Threshold", &high_V, 255, on_low_V_thresh_trackbar);
    
    while (true) {
        capture >> frame;
        flip(frame, frame, +1);
        roi_initialization();
        ///Convertion de la frame et application d'un filtre médian sur l'image convertie
        cvtColor(frame(roi), frame_hsv, COLOR_BGR2HSV);
        medianBlur(frame_hsv, frame_hsv, 3);
        
        //Seuillage hat de l'image convertie en HSV
        inRange(frame_hsv, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), hsv_threshold_output);
        
        //Dilatation de l'image seuillée pour combler les trous
        dilate(hsv_threshold_output, hsv_threshold_output, Mat(), Point(-1, -1), 3);
        
        game_initialization();
        
        finger_counter(0, 0);
        ///Affichage des fenêtres
        imshow("HSV frame", frame_hsv);
        imshow("HSV Threshold", hsv_threshold_output);
        imshow("Game", frame);
        
        ///Attente d'un choix de l'utilsateur
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)//Si l'utilisateur appuie sur q ou esc, on quitte le programme
            break;
        else if(keyboard == 'v')//sinon s'il appuie sur V, l'utilisateur valide son choix et on détermine le vainqueur
        {
            display_result(c);
        }
    }
    return 0;
}

///Fonction qui détermine les contours, l'enveloppe et le nombre de défauts
void finger_counter(int, void* )
{
    
    double largest_area = 0.0;///valeur de la surface du plus grand contour
    int largest_area_index = 0;///index du plus grand contour
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    c = 0;
    Mat drawing = Mat::zeros( hsv_threshold_output.size(), CV_8UC3 );
    Scalar color = Scalar(0,0,255);///Red
    /// Trouver les contours
    findContours(hsv_threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
    ///Recherche du contour de la main, généralement le plus grand contour en terme de surface
    if(!contours.empty())
    {
        for(int i = 0; i < contours.size(); i++)
        {
            
            if(contourArea(contours[i]) > largest_area && contourArea(contours[i]) < pow(frame.size().width/4 - 1, 2))
            {
                largest_area = contourArea(contours[i]);
                largest_area_index = i;
            }
        }
        /// Détermination de l'enveloppe du contour de la main
        vector<vector<int> >hull( contours.size() );
        vector<vector<Point> >hull_points( contours.size() );
        vector<vector<Vec4i>> defects (contours.size());
        convexHull( contours[largest_area_index], hull[largest_area_index], false );
        convexHull( contours[largest_area_index], hull_points[largest_area_index], false );
        convexityDefects(contours[largest_area_index], hull[largest_area_index], defects[largest_area_index]);
        
        ///Détermination des défauts a prendre en compte en fonction du point le plus profond et dessine le point le plus profond
        if(defects[largest_area_index].size() != 0)
        {
            for(int i = 0; i < defects[largest_area_index].size(); i++)
            {
                if(defects[largest_area_index][i][3] > 8000)
                {
                    circle(drawing, contours[largest_area_index][defects[largest_area_index][i][2]], 3, Scalar(255, 0, 0), 8);
                    c++;
                }
            }
        }
        
        ///Dessine l'enveloppe, les contours et affiche le choix de l'utilisateur
        if(contourArea(hull_points[largest_area_index]) < pow(frame.size().width/4 - 1, 2))
        {
            drawContours( drawing, contours, largest_area_index, color, 1, 8, vector<Vec4i>(), 0, Point() );
            drawContours( drawing, hull_points, largest_area_index, Scalar(255, 255, 0), 1, 8, vector<Vec4i>(), 0, Point() );
            if(c == 4)
                putText(frame, "Feuille", Point(0,frame.size().height/8), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,255), 2, false);
            else if(c == 2 || c == 1)
                putText(frame, "Ciseaux", Point(0,frame.size().height/8), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,255), 2, false);
            else if(c == 0 && defects[largest_area_index].size())
                putText(frame, "Pierre", Point(0,frame.size().height/8), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,255), 2, false);
        }
        //Defect en indice car defects contient les indices des points de départ, arrivée et le plus profond dans la liste d'indices du contour d'origine
        
        ///Affiche l'image avec les contours, l'enveloppe et les défauts de convexité
        namedWindow( "Hull", WINDOW_AUTOSIZE );
        imshow( "Hull", drawing );
    }
}
