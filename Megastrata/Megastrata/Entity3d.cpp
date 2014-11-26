#include "Entity3d.h"

#include "World.h"
#include "Vehicle.h"

int Entity3d::____last_id = 0;

Entity3d::Entity3d(void)
{
	id = ____last_id++;
	rendered_type = RENDERED_NONE;

	Connectivity.ConnectionsValue = 0;

	xpos = ypos = 0.0f;
	width = length = height = 1;

	GeneratedByID = -1;
	m_entityOwner = NULL;

	memset(variables, 0, sizeof(variables));

	//variable = 0;
}


void Entity3d::Initialize(int generatorID, int w, int l, int h, EntityRenderedType renderType)
{
	GeneratedByID = generatorID;

	m_bakingCollisions = false;

	width = ABS(w);
	length = ABS(l);
	height = ABS(h);
	rendered_type = renderType;
	Connectivity.ConnectionsValue = 0;
	m_entityOwner = NULL;

	m_collidables.ClearCollidables();

	memset(variables, 0, sizeof(variables));
}

vector<ScriptableEntity*> Entity3d::Render(ConnectionStruct externalConnections, ConnectionStruct adjacentTunnels, EntityRenderedType *adjTypes, int textureGroup, bool shouldBakeCollisions, float *heights)
{
	m_bakingCollisions = shouldBakeCollisions;
	switch(rendered_type)
	{
	case RENDERED_PADDING:
		//nothing.
		break;
	case RENDERED_JUNCTIONBOXES:
		RenderJunctionBoxes(externalConnections, adjacentTunnels, textureGroup, heights);
		break;
	case RENDERED_SHAFT:
		return RenderShaft(externalConnections, adjacentTunnels, textureGroup, heights, adjTypes);
		break;

	case RENDERED_DUMMY:
	default:
		RenderDummy();
		break;
	}

	return vector<ScriptableEntity*>();
}

void Entity3d::RenderDummy()
{
	if(m_bakingCollisions) return;

	{
		AutoMatrixStack stack;

		glTranslatef(xpos, ypos, 0);
		glScalef(width* GRID_SIZE, length* GRID_SIZE, height* GRID_SIZE);
		glutSolidCube(0.9f);
	}
}

//connection width also is the width/length of the central box
//everything else is based off of this. Tweak as needed.
#define CONNECTION_WIDTH		0.35f

#define SHAFT_HEIGHT (PHYSICAL_HEIGHT_FACTOR)
#define SHAFT_RADIUS (1/(9.0f))
#define CAR_SHAFT_WIDTH (1.5f/27.0f)
#define RAILING_HEIGHT (0.05f)

vector<ScriptableEntity*> Entity3d::RenderShaft(ConnectionStruct externalConnections, ConnectionStruct adjacentTunnels, int textureGroup, float *heights, EntityRenderedType *adjTypes)
{
	AutoMatrixStack stack;
	Vector3d corners[4];
	glTranslatef(xpos, ypos, 0);
	glScalef(width * GRID_SIZE, length * GRID_SIZE, 1.0);

	vector<ScriptableEntity*> scriptables;

	{
		//central core
		AutoMatrixStack stack;
		glTranslatef(0,0,-SHAFT_HEIGHT/2);

		if(textureGroup == 0)
			glutSolidCylinder(SHAFT_RADIUS - 3*EPSILON, SHAFT_HEIGHT, 40, 10);

		stack.Pop();

		for(int i = 0; i < 4; i++)
		{
			float current_angle = -90*i + 90.0f;

			{
				//add shafts
				AutoMatrixStack stack;
				glRotatef(current_angle, 0, 0, 1.0f);
				glTranslatef(SHAFT_RADIUS + CAR_SHAFT_WIDTH, 0, 0);

				//3 sides of shaft.
				if(textureGroup == 1)
				{
					for(int q = -1; q < 2; q++)
					{
						AutoMatrixStack stack;
						glRotatef(90 * q, 0, 0, 1.0f);
						glTranslatef(-CAR_SHAFT_WIDTH, 0, 0);
						corners[0] = Vector3d(0, -CAR_SHAFT_WIDTH,	-SHAFT_HEIGHT/2);
						corners[1] = Vector3d(0, -CAR_SHAFT_WIDTH,	SHAFT_HEIGHT/2);
						corners[2] = Vector3d(0, CAR_SHAFT_WIDTH,	SHAFT_HEIGHT/2);
						corners[3] = Vector3d(0, CAR_SHAFT_WIDTH,	-SHAFT_HEIGHT/2);
						CreateQuad(corners, 3, 3);
					}
				}

				if(m_bakingCollisions) //only do this once
				{
					Vector3d thispos;
					//get current matrix
					float modelview[16];
					glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
					thispos = thispos.Transform(modelview);
					
					//create scriptables
					Vehicle *vehicle = new Vehicle();
					vehicle->Initialize(thispos, Vector3d(2*CAR_SHAFT_WIDTH - 0.01f, 2*CAR_SHAFT_WIDTH - 0.01f, 3*CAR_SHAFT_WIDTH) * 3.0f, current_angle);
					scriptables.push_back(vehicle);
				}

				if(textureGroup == 2 || m_bakingCollisions)
				{
					
					//extra platform
					glTranslatef(CAR_SHAFT_WIDTH, 0, 0);

					if(textureGroup == 2)
					{
						corners[0] = Vector3d(0, -CAR_SHAFT_WIDTH, 0);
						corners[1] = Vector3d(0, CAR_SHAFT_WIDTH, 0);
						corners[2] = Vector3d(CAR_SHAFT_WIDTH, CAR_SHAFT_WIDTH, 0);
						corners[3] = Vector3d(CAR_SHAFT_WIDTH, -CAR_SHAFT_WIDTH, 0);
						CreateQuad(corners, 1, 1);
					}

					//extra mini-railings :D (so cuuuuute)
					{
						AutoMatrixStack stack;
						float railheight;
						if(m_bakingCollisions)
							railheight = variables[0]; //set to height of room
						else
							railheight = RAILING_HEIGHT;
						glTranslatef(0, -CAR_SHAFT_WIDTH, 0);
						corners[0] = Vector3d(0, 0, 0);
						corners[1] = Vector3d(0, 0, railheight);
						corners[2] = Vector3d(CAR_SHAFT_WIDTH, 0, railheight);
						corners[3] = Vector3d(CAR_SHAFT_WIDTH, 0, 0);
						CreateQuad(corners, 1, 1);
						
						//reverse
						glTranslatef(0, 2*CAR_SHAFT_WIDTH, 0);
						corners[0] = Vector3d(CAR_SHAFT_WIDTH, 0, 0);
						corners[1] = Vector3d(CAR_SHAFT_WIDTH, 0, railheight);
						corners[2] = Vector3d(0, 0, railheight);
						corners[3] = Vector3d(0, 0, 0);
						CreateQuad(corners, 1, 1);
					}
				}

			}

			{
				AutoMatrixStack stack;
				//add platforms

				if(textureGroup == 2 || m_bakingCollisions)
				{
					{
						AutoMatrixStack stack;
						glRotatef(current_angle, 0, 0, 1.0f);

						glTranslatef(2.5f / 9.0f, 0, 0);

						//railings
						{
							AutoMatrixStack stack;
							float railheight;
							if(m_bakingCollisions)
								railheight = variables[0]; //set to height of room
							else
								railheight = RAILING_HEIGHT;
							glTranslatef(0, 0.5f / 9.0f, 0);
							corners[0] = Vector3d(0, 2.0f / 9.0f, 0);
							corners[1] = Vector3d(0, 2.0f / 9.0f, railheight);
							corners[2] = Vector3d(0, 0, railheight);
							corners[3] = Vector3d(0, 0, 0);
							CreateQuad(corners, 1, 1);
							glTranslatef(0,-3.0f/9.0f,0);
							CreateQuad(corners, 1, 1);
						}

						if(!m_bakingCollisions)
						{
							//floor and ceiling not included in collisions

							glTranslatef(0, -4.5f / 9.0f, 0);

							corners[0] = Vector3d(0, 0, 0);
							corners[1] = Vector3d(0, 7.0f / 9.0f, 0);
							corners[2] = Vector3d(2.0f/9.0f, 7.0f / 9.0f, 0);
							corners[3] = Vector3d(2.0f/9.0f, 0, 0);
							CreateQuad(corners, 2, 2);

							glTranslatef(0,0,variables[0]);

							corners[0] = Vector3d(0, 0, 0);
							corners[1] = Vector3d(0, 7.0f / 9.0f, 0);
							corners[2] = Vector3d(2.0f/9.0f, 7.0f / 9.0f, 0);
							corners[3] = Vector3d(2.0f/9.0f, 0, 0);
							CreateQuad(corners, 2, 2);
						}
					}

					//do walls where no exits exist (collisions too)
					if(!externalConnections.GetByIndex(i))
					{
						float thisheight = variables[0];
						AutoMatrixStack stack;
						glRotatef(current_angle, 0, 0, 1.0f);

						glTranslatef(0.5f, 0, 0);

						corners[0] = Vector3d(0, -0.5f, 0);
						corners[1] = Vector3d(0, -0.5f, thisheight);
						corners[2] = Vector3d(0, 0.5f, thisheight);
						corners[3] = Vector3d(0, 0.5f, 0);
						CreateQuad(corners, 2, 2);
						
						//make a back-shield too
						if(m_bakingCollisions)
						{
							AutoMatrixStack stack;
							glRotatef(180, 0,0,1.0f);
							CreateQuad(corners, 1, 1);
						}
					}

					if(externalConnections.GetByIndex(i) && adjTypes[i] != RENDERED_SHAFT)
					{
						float thisheight = variables[0];

						if(thisheight - heights[i] > EPSILON)
							thisheight = heights[i];

						float third = 1.0f / 6.0f;
						if(adjacentTunnels.GetByIndex(i))
							third -= ((1.0 - CONNECTION_WIDTH)/2.0f) / 3.0f;

						AutoMatrixStack stack;
						glRotatef(current_angle, 0, 0, 1.0f);

						glTranslatef(0.5f, 0, 0);

						corners[0] = Vector3d(0, -0.5f, 0);
						corners[1] = Vector3d(0, -0.5f, thisheight);
						corners[2] = Vector3d(0, -third, thisheight);
						corners[3] = Vector3d(0, -third, 0);
						CreateQuad(corners, 2, 2);
						//make a back-shield too
						if(m_bakingCollisions)
						{
							AutoMatrixStack stack;
							glRotatef(180, 0,0,1.0f);
							CreateQuad(corners, 1, 1);
						}

						corners[0] = Vector3d(0, third, 0);
						corners[1] = Vector3d(0, third, thisheight);
						corners[2] = Vector3d(0, 0.5f, thisheight);
						corners[3] = Vector3d(0, 0.5f, 0);
						CreateQuad(corners, 2, 2);

						//make a back-shield too
						if(m_bakingCollisions)
						{
							AutoMatrixStack stack;
							glRotatef(180, 0,0,1.0f);
							CreateQuad(corners, 1, 1);
						}
					}

					{
						float thisheight = variables[0];
						AutoMatrixStack stack;
						glRotatef(current_angle, 0, 0, 1.0f);

						if(!m_bakingCollisions)
						{
							if(externalConnections.GetByIndex(i) && (thisheight - heights[i] > EPSILON))
							{
								//this cell is taller than the next one over.
								AutoMatrixStack stack;
								float difference = thisheight - heights[i];
								glTranslatef(0.5f,0, heights[i]);

								corners[0] = Vector3d(0, -0.5f, 0);
								corners[1] = Vector3d(0, -0.5f, difference);
								corners[2] = Vector3d(0, 0.5f, difference);
								corners[3] = Vector3d(0, 0.5f, 0);
								CreateQuad(corners, 2, 2);
							}
						}
					}
				}
			}
		}
	}

	return scriptables;
}



//Connection length must adjust accordingly, to fit within the grid square.
#define CONNECTION_LENGTH		((GRID_SIZE - CONNECTION_WIDTH)/2)

//the translation must be performed in terms of CONNECTION_LENGTH.
//Here we translate 1/2 the length of the connection (achieving the edge)
//and then we move it a further 1/2 of the connection width (moving it out of the central box).
//kind of disgusting. But this is the precise translation needed.
#define TRANSLATE_CONNECTION	(0.5f + ((CONNECTION_WIDTH/2) / CONNECTION_LENGTH))

void Entity3d::RenderJunctionBoxes(ConnectionStruct externalConnections, ConnectionStruct adjacentTunnels, int textureGroup, float *heights)
{
	//position in correct location
	AutoMatrixStack stack;

	//use variable for height
	glScalef(1.0f, 1.0f, variables[0]);
	glTranslatef(xpos, ypos, 0.5);

	//CLARMP!
	variables[1] = MAX(0.0f, variables[1]);
	variables[1] = MIN(1.0f, variables[1]);

	//compute bevel info
	BevelInfo bevel;
	BevelInfo *thesame = &bevel;
	*((unsigned char*)(thesame)) = 0xFF * variables[1];

	//compute unit of bevel
	float max_bevel = CONNECTION_LENGTH / 2.0f;
	float bevel_unit = max_bevel / 3.0f;

	//big box
	if(Connectivity.ConnectionsValue == 0)
	{
		AutoMatrixStack stack;

			DrawTopAndBottom(textureGroup);

			for(int i = 0; i < 4; i++)
			{
				//iterate four directions.
				int delx = dir_iter[i];
				int dely = dir_iter[(i + 1) % 4];

				if(!externalConnections.GetByIndex(i))
					DrawSideFace((Direction)i, textureGroup);
				else if(adjacentTunnels.GetByIndex(i))
				{
					//draw two extra walls
					AutoMatrixStack stack;

					glScalef(1.0f + (ABS(dely) * (CONNECTION_LENGTH - 1.0f)), 1.0f + (ABS(delx) * (CONNECTION_LENGTH - 1.0f)), 1.0f);

					glTranslatef(TRANSLATE_CONNECTION * dely, TRANSLATE_CONNECTION * delx, 0);
					
					if((variables[0] > heights[i]))
					{
						//scale down extra wall sizes in order to prevent overlap with the height-fix below
						glTranslatef(0,0, -0.5 + heights[i]/(2*variables[0]));
						glScalef(1.0f, 1.0f, heights[i]/variables[0]);
					}
					DrawSideFace((Direction)i, textureGroup);

					glTranslatef(2*TRANSLATE_CONNECTION*(-dely), 2*TRANSLATE_CONNECTION*(-delx), 0);
					DrawSideFace((Direction)i, textureGroup);
				}

				if(externalConnections.GetByIndex(i) && (variables[0] > heights[i]))
				{
					//this cell is taller than the next one over.
					AutoMatrixStack stack;
					float difference = variables[0] - heights[i];
					glTranslatef(0,0, heights[i]/(2*variables[0]));
					glScalef(1.0f, 1.0f, difference/variables[0]);
					DrawSideFace((Direction)i, textureGroup);
				}
			}
	}
	else
	{
		{
			//central box
			AutoMatrixStack stack;

			glScalef(CONNECTION_WIDTH, CONNECTION_WIDTH, 1.0f);

			DrawTopAndBottom(textureGroup);

			for(int i = 0; i < 4; i++)
			{
				if(!Connectivity.Connections.GetByIndex(i))
					DrawSideFace((Direction)i, textureGroup);
			}
		}


		for(int i = 0; i < 4; i++)
		{

			//this one goes by CORNERS, not by directions.
			if(Connectivity.Connections.GetByIndex(i) && Connectivity.Connections.GetByIndex(i+1) && (bevel.GetByIndex(i) != 0))
			{
				//do beveled edge
				float bevelDis = bevel_unit * bevel.GetByIndex(i);
				float connection_length = (CONNECTION_LENGTH - bevelDis);
				float translate_connection = (0.5f + ((bevelDis + CONNECTION_WIDTH/2) / connection_length));

				int delx = dir_iter[i];
				int dely = dir_iter[(i + 1) % 4];

				//regardless, we must cap the end, and draw ceiling/floor for this quadrant.
				{
					AutoMatrixStack stack;

					glScalef(CONNECTION_LENGTH * ABS(delx) + CONNECTION_WIDTH * ABS(dely), CONNECTION_LENGTH * ABS(dely) + CONNECTION_WIDTH * ABS(delx), 1.0f);
					glTranslatef(TRANSLATE_CONNECTION*delx, TRANSLATE_CONNECTION*dely, 0);
					DrawTopAndBottom(textureGroup);
					if(!externalConnections.GetByIndex(i))
						DrawSideFace((Direction)i, textureGroup);

					if(externalConnections.GetByIndex(i) && (variables[0] > heights[i]))
					{
						//this cell is taller than the next one over.
						AutoMatrixStack stack;
						float difference = variables[0] - heights[i];
						glTranslatef(0,0, heights[i]/(2*variables[0]));
						glScalef(1.0f, 1.0f, difference/variables[0]);
						DrawSideFace((Direction)i, textureGroup);
					}
				}


				for(int k = 0; k < 2; k++)
				{
					if(textureGroup == k+1)
					{
						AutoMatrixStack stack;
							//Triangle patch
							glRotatef( -i*90 + 45, 0, 0, 1.0f);
							glTranslatef(SQRT_2 * CONNECTION_WIDTH/2, 0, 0);
							
							glRotatef( 180*(k+1), 1.0f, 0, 0);
							glTranslatef(0,0,-0.5f);
							glScalef(SQRT_2 * bevelDis / 2, SQRT_2 * bevelDis, 1.0f);
							glRotatef(180, 1.0f, 0, 0);

							//sooooo ug-layyy 
							glBegin(GL_TRIANGLES);
								glTexCoord2f(0.0f, 0.0f); glNormal3f(0,0,-1.0f); glVertex3f(0, 0, 0.0f);
								glTexCoord2f(1.0f, 0.5f); glNormal3f(0,0,-1.0f); glVertex3f(1.0f, 0.5f, 0.0f);
								glTexCoord2f(1.0f, -0.5f); glNormal3f(0,0,-1.0f); glVertex3f(1.0f, -0.5f, 0.0f);
							glEnd();
					}
				}

				//we perform raw DoQuads here - so check that we're in the right mood/mode
				if(textureGroup == 0 || m_bakingCollisions)
				{
					{
						AutoMatrixStack stack;

						glScalef(connection_length * ABS(delx) + CONNECTION_WIDTH * ABS(dely), connection_length * ABS(dely) + CONNECTION_WIDTH * ABS(delx), 1.0f);
						glTranslatef(translate_connection*delx, translate_connection*dely, 0);
						DrawSideFace((Direction) (((i + 1) + 4) % 4), textureGroup);
					}

					delx = dir_iter[(i + 1) % 4];
					dely = dir_iter[(i + 2) % 4];

					{
						AutoMatrixStack stack;
						glScalef(connection_length * ABS(delx) + CONNECTION_WIDTH * ABS(dely), connection_length * ABS(dely) + CONNECTION_WIDTH * ABS(delx), 1.0f);
						glTranslatef(translate_connection*delx, translate_connection*dely, 0);
						DrawSideFace((Direction) (((i) + 4) % 4), textureGroup);
					}

					{
						AutoMatrixStack stack;

						//draw beveled edge! Draw eastward and then rotate into position.
						glRotatef( -i*90 + 45, 0, 0, 1.0f);
						glScalef(1.0f, bevelDis * SQRT_2, 1.0f);

						glTranslatef(SQRT_2 * CONNECTION_WIDTH/2 + (bevelDis * SQRT_2) / 2, 0, 0);

						glRotatef(-90, 0, 1.0f, 0); 
						glRotatef(180, 1.0f, 0, 0);
						DoQuad();
					}
				}

			}
			else // no bevel
			{
				if(Connectivity.Connections.GetByIndex(i)) //first dir
				{
					//do first dir
					int delx = dir_iter[i];
					int dely = dir_iter[(i + 1) % 4];

					{
						AutoMatrixStack stack;

						glScalef(CONNECTION_LENGTH * ABS(delx) + CONNECTION_WIDTH * ABS(dely), CONNECTION_LENGTH * ABS(dely) + CONNECTION_WIDTH * ABS(delx), 1.0f);
						glTranslatef(TRANSLATE_CONNECTION*delx, TRANSLATE_CONNECTION*dely, 0);
						DrawTopAndBottom(textureGroup);
						DrawSideFace((Direction) (((i + 1) + 4) % 4), textureGroup);
						if(!externalConnections.GetByIndex(i))
							DrawSideFace((Direction)i, textureGroup);

						if(externalConnections.GetByIndex(i) && (variables[0] > heights[i]))
						{
							//this cell is taller than the next one over.
							AutoMatrixStack stack;
							float difference = variables[0] - heights[i];
							glTranslatef(0,0, heights[i]/(2*variables[0]));
							glScalef(1.0f, 1.0f, difference/variables[0]);
							DrawSideFace((Direction)i, textureGroup);
						}
					}
				}

				if(Connectivity.Connections.GetByIndex(i+1)) //second dir
				{
					int delx = dir_iter[(i+1) % 4];
					int dely = dir_iter[(i + 2) % 4];
					{
						AutoMatrixStack stack;

						glScalef(CONNECTION_LENGTH * ABS(delx) + CONNECTION_WIDTH * ABS(dely), CONNECTION_LENGTH * ABS(dely) + CONNECTION_WIDTH * ABS(delx), 1.0f);
						glTranslatef(TRANSLATE_CONNECTION*delx, TRANSLATE_CONNECTION*dely, 0);

						//already capped above.
						//already drew top/bottom above.
						//we're just here to finish drawing the faces:
						DrawSideFace((Direction) (((i) + 4) % 4), textureGroup);
					}
				}
			} //end if should draw bevel
		}
	}
}

void Entity3d::CreateQuad(Vector3d *corners, int tessellation_u, int tessellation_v)
{
	if(!m_bakingCollisions)
	{
		TessellatedQuad(corners, tessellation_u, tessellation_v);
	}
	else
	{
		//get current matrix
		float modelview[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
		m_collidables.AddCollisionPlane(modelview, corners[0], corners[1], corners[2], corners[3]);
	}
}

void Entity3d::DrawFace(float yangle, float zangle)
{
	AutoMatrixStack stack;
	
	glRotatef(yangle, 0, 1.0f, 0); 
	glRotatef(zangle, 1.0f, 0, 0);

	glTranslatef(0,0,+0.5);

	DoQuad();
}

void Entity3d::DoQuad()
{
	#define HALF_UNIT  (GRID_SIZE / 2)
	Vector3d corners[4];
	corners[0] = Vector3d(-HALF_UNIT, HALF_UNIT, 0);
	corners[1] = Vector3d(HALF_UNIT, HALF_UNIT, 0);
	corners[2] = Vector3d(HALF_UNIT, -HALF_UNIT, 0);
	corners[3] = Vector3d(-HALF_UNIT, -HALF_UNIT, 0);
	
	CreateQuad(corners, 2, 2);
}

void Entity3d::DrawTopAndBottom(int textureGroup)
{
	//only collide with walls for now.
	if(textureGroup == 1)// || m_bakingCollisions)
	{
		DrawFace(0);
	}

	if(textureGroup == 2)// || m_bakingCollisions)
	{
		DrawFace(180);
	}
}

void Entity3d::DrawSideFace(Direction dir, int textureGroup)
{
	if(textureGroup == 0 || m_bakingCollisions)
	{
		switch(dir)
		{
		case WEST:
			DrawFace(90, 180);
			break;
		case EAST:
			DrawFace(-90, 180);
			break;
		case NORTH:
			DrawFace(-90, -90);
			break;
		case SOUTH:
			DrawFace(90, 90);
			break;
		}
	}
}

void Entity3d::GetCollidables(CollidableBatch *batch, vector<int> *exclusion_list)
{
	if(m_entityOwner != NULL)
		m_entityOwner->GetCollidables(batch, exclusion_list);
	else
	{
		//see if we're excluded
		for(int i = 0; i < exclusion_list->size(); i++)
		{
			if(this->id == exclusion_list->at(i))
			{
				//we exist in list, leave.
				return;
			}
		}

		//add ourselves to list, add collidables.
		exclusion_list->push_back(this->id);
		m_collidables.AddCollidablesToBatch(batch);
	}
}