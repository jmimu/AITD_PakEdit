#include "alonebody.h"
#include "renderer.h"
#include <clocale>

bool body2ply(AloneFile *file)
{
    const char* filename = file->mPAKFilename;
    printf("Loading file %s\n",filename);
    char* ptr = file->mDecomprData;

    sBody* body = createBodyFromPtr(ptr);

    printf("Body loaded: %d vertices, %d groups, %d primitives\n",
           body->m_vertices.size(), body->m_groups.size(), body->m_primitives.size());

    char foutname[256+9];
    sprintf(foutname,"%s_%05d.ply",file->mPAKFilename,file->mIndex);
    AnimNuage(0,0,0,0,0,0,body);

    int num_ply_primitives=0;
    for (auto &pr: body->m_primitives)
        if (pr.m_points.size()>2) ++num_ply_primitives;

    setlocale(LC_ALL,"C");
    printf("Export ply into %s\n",foutname);
    FILE* f;
    f = fopen(foutname,"w");
    fprintf(f,"ply\n");
    fprintf(f,"format ascii 1.0\n");
    fprintf(f,"element vertex %d\n",body->m_vertices.size());
    fprintf(f,"property float32 x\n");
    fprintf(f,"property float32 y\n");
    fprintf(f,"property float32 z\n");
    fprintf(f,"element face %d\n", num_ply_primitives);
    fprintf(f,"property uchar red\n");
    fprintf(f,"property uchar green\n");
    fprintf(f,"property uchar blue\n");
    fprintf(f,"property list uchar int vertex_index\n");
    fprintf(f,"end_header\n");
    for (unsigned int i=0;i<body->m_vertices.size();i++)
    {
        //fprintf(f,"%f %f %f\n", -v.x/1000.0, -v.z/1000.0, -v.y/1000.0);
        fprintf(f,"%f %f %f\n", -pointBuffer[i*3+0]/1000.0, -pointBuffer[i*3+2]/1000.0, -pointBuffer[i*3+1]/1000.0);
    }
    for (auto &pr: body->m_primitives)
    {
        if (pr.m_points.size()>2)
        {
            fprintf(f,"%d %d %d %d ",
                    AloneFile::palette[pr.m_color*3],
                    AloneFile::palette[pr.m_color*3+1],
                    AloneFile::palette[pr.m_color*3+2],
                    pr.m_points.size());
            for (auto &pt: pr.m_points)
            {
                fprintf(f,"%d ",pt);
            }
            fprintf(f,"\n");
        }
    }
    fclose(f);
    return true;
}
