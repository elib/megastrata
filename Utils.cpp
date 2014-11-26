#include <utils.h>
#include "Megastrata/Megastrata/Vector3d.h"

//ripped off from: http://www.gamedev.net/community/forums/topic.asp?topic_id=211918
bool FileExists( const char* FileName )
{
    FILE* fp = NULL;

    //will not work if you do not have read permissions
    //to the file, but if you don't have read, it
    //may as well not exist to begin with.

    fp = fopen( FileName, "rb" );
    if( fp != NULL )
    {
        fclose( fp );
        return true;
    }

    return false;
}

void TessellatedQuad(Vector3d *corners, int tessellation_u, int tessellation_v)
{
	if(tessellation_u <= 0)
		tessellation_u = 1;

	if(tessellation_v <= 0)
		tessellation_v = 1;

	//compute normal
	Vector3d u_dir = (corners[1] - corners[0]);
	Vector3d v_dir = (corners[3] - corners[0]);
	Vector3d norm = (u_dir.Cross(v_dir)).Normalize();

	//get size of quad
	float u_dis = u_dir.Magnitude();
	float v_dis = v_dir.Magnitude();

	float u_tess_dis = u_dis * (1 / (float)tessellation_u);
	float v_tess_dis = v_dis * (1 / (float)tessellation_v);

	//cheap-ass normalizing
	u_dir = u_dir / u_dis;
	v_dir = v_dir / v_dis;

	glBegin(GL_QUADS);
	for(int i = 0; i < tessellation_u; i++)
	{
		for(int j = 0; j < tessellation_v; j++)
		{
			//top-left of current tessellated quad
			Vector3d curr = corners[0]
			+ u_dir * u_tess_dis * i
				+ v_dir * v_tess_dis * j;

			//place first vertex ...
			glTexCoord2f(u_tess_dis * i, v_tess_dis * j); glNormal3f(norm.x, norm.y, norm.z); glVertex3f(curr.x, curr.y, curr.z);

			curr = curr + u_dir * u_tess_dis;
			glTexCoord2f(v_tess_dis * (i+1), v_tess_dis * j); glNormal3f(norm.x, norm.y, norm.z); glVertex3f(curr.x, curr.y, curr.z);

			curr = curr + v_dir * v_tess_dis;
			glTexCoord2f(v_tess_dis * (i+1), v_tess_dis * (j+1)); glNormal3f(norm.x, norm.y, norm.z); glVertex3f(curr.x, curr.y, curr.z);

			curr = curr - u_dir * u_tess_dis;
			glTexCoord2f(v_tess_dis * i, v_tess_dis * (j+1)); glNormal3f(norm.x, norm.y, norm.z); glVertex3f(curr.x, curr.y, curr.z);
		}
	}
	glEnd();
}