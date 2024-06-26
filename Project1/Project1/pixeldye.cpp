#include "pixeldye.h"
#include <map>
#include <vector>


void scan(Vector4 verticeNDC[], Vector4 verticesClip[], int size, unsigned char* data, unsigned char* imagedata, Vector4 verticeuv[], int width, int height, double* zbuffer) {
    int Max_Y = 0;
    int Min_Y = (int)SCR_HEIGHT;

    std::vector<std::vector<int>> vertices;
    std::vector <std::vector<float>> veticeDivideW;

    for (int i = 0; i < size; i++) {
        std::vector<int> v;
        v.push_back(int(verticeNDC[i].x + 0.5));
        v.push_back(int(verticeNDC[i].y + 0.5));
        vertices.push_back(v);
        std::vector<float> v1;
        v1.push_back(verticeuv[i].x / verticesClip[i].w);
        v1.push_back(verticeuv[i].y / verticesClip[i].w);
        v1.push_back(1 / verticesClip[i].w);
        v1.push_back(verticeNDC[i].z / verticesClip[i].w);
        veticeDivideW.push_back(v1);

    }

    for (int i = 0; i < size; i++) {
        if (vertices[i][1] > Max_Y)
            Max_Y = vertices[i][1];
        if (vertices[i][1] < Min_Y)
            Min_Y = vertices[i][1];
            
    }


    AET* pAET = new AET;
    pAET->next = NULL;

    std::map<int, NET*> m;

    for (int j = 0; j < size; j++) {

          if (vertices[(j - 1 + size) % size][1] > vertices[j][1])
                {
                    NET* p = new NET;
                    p->x = vertices[j][0];
                    p->ymax = vertices[(j - 1 + size) % size][1];
                    float DX = vertices[(j - 1 + size) % size][0] - vertices[j][0];
                    float DY = vertices[(j - 1 + size) % size][1] - vertices[j][1];
                    p->dx = DX / DY;
                    NET* q = m[vertices[j][1]];
                    if (q == NULL) {
                        q = new NET;
                        q->next = p;
                        p->next = NULL;
                        m[vertices[j][1]] = q;

                    }
                    else {
                        p->next = q->next;
                        q->next = p;
                    }
                   
                }
          if (vertices[(j + 1 + size) % size][1] > vertices[j][1])
                {
                    NET* p = new NET;
                    p->x = vertices[j][0];
                    p->ymax = vertices[(j + 1 + size) % size][1];
                    float DX = vertices[(j + 1 + size) % size][0] - vertices[j][0];
                    float DY = vertices[(j + 1 + size) % size][1] - vertices[j][1];
                    p->dx = DX / DY;
                    
                    NET* q = m[vertices[j][1]];
                    if (q == NULL) {
                        q = new NET;
                        q->next = p;
                        p->next = NULL;
                        m[vertices[j][1]] = q;
                    }
                    else {
                        p->next = q->next;
                        q->next = p;
                    }
                }
   
    }


    for (int i = Min_Y; i <= Max_Y; i++) {

        NET* p = pAET->next;

        while (p) {
            p->x = p->x + p->dx;
            p = p->next;
        }

        AET* tq = pAET;
        NET* ap = m[i];
        if (ap) ap = ap->next;
        while (ap) {
            while (tq->next != NULL && (tq->next->x + tq->next->dx )<= (ap->x +ap->dx))
                tq = tq->next;
            NET* t = ap->next;
            ap->next = tq->next;
            tq->next = ap;
            ap = t;
            tq = pAET;
        }

        AET* q = pAET;
        p = q->next;
        while (p) {
            if (p->ymax == i) {
                q->next = p->next;
                delete p;
                p = q->next;
            }
            else {
                q = q->next;
                p = q->next;
            }
        }

        p = pAET->next;
        while (p && p->next != NULL) {
            for (int j = p->x; j <= p->next->x; j++) {
                // p 0 1
                double s1 = abs((j - vertices[1][0]) * (vertices[0][1] - vertices[1][1]) - (vertices[0][0] - vertices[1][0]) * (i - vertices[1][1]));
                // p 1 2
                double s2 = abs((j - vertices[2][0]) * (vertices[1][1] - vertices[2][1]) - (vertices[1][0] - vertices[2][0]) * (i - vertices[2][1]));
                // p 0 2
                double s3 = abs((j - vertices[2][0]) * (vertices[0][1] - vertices[2][1]) - (vertices[0][0] - vertices[2][0]) * (i - vertices[2][1]));
                double total = s1 + s2 + s3;
                double r1 = s1 / total;
                double r2 = s2 / total;
                double r3 = s3 / total;
                double u = (r1 * veticeDivideW[2][0] + r2 * veticeDivideW[0][0] + r3 * veticeDivideW[1][0]);
                double v = (r1 * veticeDivideW[2][1] + r2 * veticeDivideW[0][1] + r3 * veticeDivideW[1][1]);
                double inversedW = (r1 * veticeDivideW[2][2] + r2 * veticeDivideW[0][2] + r3 * veticeDivideW[1][2]);
                double zDivideW = (r1 * veticeDivideW[2][3] + r2 * veticeDivideW[0][3] + r3 * veticeDivideW[1][3]);
                double z = zDivideW / inversedW;
                int index = i * SCR_WIDTH + j;
                int t = index * 3;
                if ( z < zbuffer[index]) {
                    Color32 color = search(u / inversedW, v / inversedW, width, height, imagedata);
                    data[t] = color.r;
                    data[t + 1] = color.g;
                    data[t + 2] = color.b;
                    zbuffer[index] = z;
                }
                
            }
            p = p->next->next;
        }
    }

}

Color32 search(float u, float v, int width, int height, unsigned char* imagedata) {
    float x = u * width;
    float y = v * height;

    float top = int(y) + 0.5;
    float bottom = top - 1.0;
    float right = int(x) + 0.5;
    float left = right - 1.0;
    float xToLeft = x - left;
    float xToRight = x - right;
    float yToTop = y - top;
    float yToBottom = y - bottom;

    float distance1 = std::sqrt(xToLeft * xToLeft + yToBottom * yToBottom);
    float distance2 = std::sqrt(xToRight * xToRight + yToBottom * yToBottom);
    float distance3 = std::sqrt(xToLeft * xToLeft + yToTop * yToTop);
    float distance4 = std::sqrt(xToRight * xToRight + yToTop * yToTop);

    int leftbottom = (int(bottom) * width + int(left)) * 3;
    int rightbottom = (int(bottom) * width + int(right)) * 3;
    int lefttop = (int(top) * width + int(left)) * 3;
    int righttop = (int(top) * width + int(right)) * 3;
    float total = distance1 + distance2 + distance3 + distance4;
    
    float red = (distance1 * imagedata[leftbottom] + distance2 * imagedata[rightbottom] + distance3 * imagedata[lefttop] + distance4 * imagedata[righttop]) / total;
    float green = (distance1 * imagedata[leftbottom + 1] + distance2 * imagedata[rightbottom + 1] + distance3 * imagedata[lefttop + 1] + distance4 * imagedata[righttop + 1]) / total;
    float blue = (distance1 * imagedata[leftbottom + 2] + distance2 * imagedata[rightbottom + 2] + distance3 * imagedata[lefttop + 2] + distance4 * imagedata[righttop + 2]) / total;

    return {(unsigned char)red, (unsigned char)green, (unsigned char)blue, 0};
}

