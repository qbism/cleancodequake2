#if 0
/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
This source file is contained as part of CleanCode Quake2, a project maintained
by Paril, to 'clean up' and make Quake2 an easier source base to read and work with.

You may use any part of this code to help create your own bases and own mods off
this code if you wish. It is under the same license as Quake 2 source (as above),
therefore you are free to have to fun with it. All I ask is you email me so I can
list the mod on my page for CleanCode Quake2 to help get the word around. Thanks.
*/

//
//

#include "cc_local.h"

#include <windows.h>
#include <dMatrix.h>
#include <GL/GL.h>

NewtonWorld *nWorld;

bool inited = false;

DWORD TimeStart, TimeCurrent;

void SetMeshTransformEvent(const NewtonBody* body, const float* matrix)
{
	edict_t *attached = (edict_t*)NewtonBodyGetUserData (body);
	// copy the matrix into an irrlicht matrix4
	mat4x4_t mat;
	dMatrix haha;

	memcpy(mat, matrix, sizeof(float)*16);

	// Retreive the user data attached to the newton body
	/*ISceneNode *tmp = (ISceneNode *)NewtonBodyGetUserData(body);
	if (tmp)
	{
		// Position the node
		tmp->setPosition(mat.getTranslation());		// set position
		tmp->setRotation(mat.getRotationDegrees());	// and rotation
	}*/
	//gi.dprintf ("%f %f %f\n", mat[12], mat[13], mat[14]);
	attached->state.origin[0] = mat[12];
	attached->state.origin[2] = mat[13];
	attached->state.origin[1] = mat[14];

	NewtonGetEulerAngle (&mat[0], &attached->state.angles[0]);
	attached->state.angles[0] = RAD2DEG(attached->state.angles[0]);
	attached->state.angles[1] = RAD2DEG(attached->state.angles[1]) + 90;
	attached->state.angles[2] = RAD2DEG(attached->state.angles[2]);
	//gi.dprintf ("%f %f %f\n", attached->state.angles[0], attached->state.angles[1], attached->state.angles[2]);


	//Test_MatrixToAngles(mat, attached->state.angles);

	//mat3x3_t ma;
	//Matrix4_Matrix3 (mat, ma);

	//vec3_t angles;
	//Matrix3_Angles (ma, angles);

	//gi.dprintf ("%f %f %f\n", angles[0], angles[1], angles[2]);
	//Vec3Copy (angles, attached->state.angles);

    // Assuming the angles are in radians.
	/*if (mat[10] > 0.998) { // singularity at north pole
		attached->state.angles[0] = atan2(mat[2],mat[22]);
		attached->state.angles[1] = M_PI/2;
		attached->state.angles[2] = 0;
	}
	else if (mat[10] < -0.998) { // singularity at south pole
		attached->state.angles[0] = atan2(mat[2],mat[22]);
		attached->state.angles[1] = -M_PI/2;
		attached->state.angles[2] = 0;
	}
	else
	{
		attached->state.angles[0] = atan2(-mat[20],mat[0]);
		attached->state.angles[1] = atan2(-mat[12],mat[11]);
		attached->state.angles[2] = asin(mat[10]);
	}
	attached->state.angles[0] = RAD2DEG(attached->state.angles[0]);
	attached->state.angles[1] = RAD2DEG(attached->state.angles[1]);
	attached->state.angles[2] = RAD2DEG(attached->state.angles[2]);*/

	gi.linkentity(attached);
}

void ApplyForceAndTorqueEvent (const NewtonBody* body) 
{ 
	float mass; 
	float Ixx; 
	float Iyy; 
	float Izz; 
	float force[3]; 
	float torque[3]; 

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz); 

	force[0] = 0.0f; 
	force[1] = -250 * mass; 
	force[2] = 0.0f; 

	torque[0] = 0.0f; 
	torque[1] = 0.0f; 
	torque[2] = 0.0f; 

	NewtonBodyAddForce (body, force); 
	NewtonBodyAddTorque (body, torque); 

	edict_t *applied = (edict_t*)NewtonBodyGetUserData(body);
	if (applied->air_finished)
	{
		applied->air_finished = 0;

		vec3_t forward;

		Angles_Vectors (applied->owner->client->v_angle, forward, NULL, NULL);
		Vec3Scale (forward, 35 * mass, forward);

		float temp = forward[1];
		forward[1] = forward[2];
		forward[2] = temp;

		NewtonBodySetVelocity (body, &forward[0]);
	}
}

NewtonCollision *LoadInCollision (char *modelName);
NewtonCollision *MGCollision = NULL;

NewtonBody *MakeAStupidBox (edict_t *ent, NewtonCollision *collision, float x, float y, float z)
{
	NewtonBody *body = NewtonCreateBody(nWorld, collision);

	//float Ixx = 10.0f * (16*16 + 8*8) / 12;
	//float Iyy = 10.0f * (16*16 + 8*8) / 12;
	//float Izz = 10.0f * (16*16 + 16*16) / 12;

	NewtonBodySetMassMatrix (body, 10.0f, 150, 150, 150);
	NewtonBodySetTransformCallback(body, SetMeshTransformEvent);
	NewtonBodySetForceAndTorqueCallback(body, ApplyForceAndTorqueEvent);

	mat4x4_t mat;
	Matrix4_Identity (mat);
	Matrix4_Translate (mat, x, y, z);
	//Matrix4_Rotate (mat, 30, 1, 1, 0);
	NewtonBodySetMatrix (body, &mat[0]);

	edict_t *init = G_Spawn();
	init->state.origin[0] = x;
	init->state.origin[1] = y;
	init->state.origin[2] = z;
	//Vec3Set (init->mins, -50, -50, -50);
	//Vec3Set (init->maxs, 50, 50, 50);
	//init->solid = SOLID_BBOX;
	Vec3Copy (init->state.origin, init->state.oldOrigin);
	init->state.modelIndex = ModelIndex("models/crate.md2");
	init->movetype = MOVETYPE_NONE;
	gi.linkentity(init);
	NewtonBodySetUserData (body, (void*)init);
	init->newtonBody = body;

	if (ent && Q_stricmp(gi.argv(1), "force") == 0)
	{
		init->owner = ent;	
		init->air_finished = 1;
	}
	else
		init->air_finished = 0;

	if (ent)
	{
		//Matrix4_Rotate (mat, ent->state.angles[1], 0, 1, 0);
		//NewtonBodySetMatrix (body, &mat[0]);
	}

	return body;
}

NewtonBody *MakeARope (NewtonCollision *collision, float x, float y, float z)
{
	NewtonBody *body = NewtonCreateBody(nWorld, collision);

	NewtonBodySetMassMatrix (body, 60.0f, 150.0f, 150.0f, 150.0f);
	NewtonBodySetTransformCallback(body, SetMeshTransformEvent);
	NewtonBodySetForceAndTorqueCallback(body, ApplyForceAndTorqueEvent);

	mat4x4_t mat;
	Matrix4_Identity (mat);
	Matrix4_Translate (mat, x, y, z);
	//Matrix4_Rotate (mat, 30, 1, 1, 0);

	NewtonBodySetMatrix (body, &mat[0]);

	edict_t *init = G_Spawn();
	init->state.origin[0] = mat[12];
	init->state.origin[1] = mat[13];
	init->state.origin[2] = mat[14];
	init->state.modelIndex = ModelIndex("models/rope/tris.md2");
	gi.linkentity(init);
	NewtonBodySetUserData (body, (void*)init);

	return body;
}

NewtonCollision *glob;

typedef struct bspPoly_s
{
	uint32	numVerts;
	vec3_t			*Verts;
} bspPoly_t;

typedef struct bspMesh_s
{
	uint32	numMeshes;
	bspPoly_t		*BSPPolys;
} bspMesh_t;

#include <string>
using namespace std;

void InitNewton ()
{
	// Init newton
	nWorld = NewtonCreate(NULL, NULL);

	vec3_t mins = {-4096,-4096,-4096};
	vec3_t maxs = {4096,4096,4096};
	NewtonSetWorldSize(nWorld, &mins[0], &maxs[0]);

	// Set up default material properties for newton
	//sint32 i = NewtonMaterialGetDefaultGroupID(nWorld);
	//NewtonMaterialSetDefaultFriction   (nWorld, i, i, 0.8f, 0.4f);
	//NewtonMaterialSetDefaultElasticity (nWorld, i, i, 0.3f);
	//NewtonMaterialSetDefaultSoftness   (nWorld, i, i, 0.05f);
	//NewtonMaterialSetCollisionCallback (nWorld, i, i, NULL, NULL, NULL, NULL);

 	mat4x4_t mat;
	Matrix4_Identity (mat);
	Matrix4_Translate (mat, 0, 0, 0);
	Matrix4_Rotate (mat, 90, 1, 0, 0);

	NewtonCollision *boxColl = NewtonCreateBox (nWorld, 26, 26, 26, &mat[0]);
	//NewtonCollision *boxColl = NewtonCreateSphere (nWorld, 50, 50, 50, NULL);
	//NewtonCollision *boxColl = NewtonCreateBox (nWorld, 4, 16, 34, NULL);
	glob = boxColl;

	/*NewtonCollision *ropeColl = NewtonCreateBox(nWorld, 3, 6, 3, NULL);

	glob = boxColl;
	NewtonBody *b = MakeAStupidBox (NULL, boxColl, 0, -15, 0);

	// Ropes
	NewtonBody *Parent = b, *NewChild;
	float offset = -16;
	vec3_t pivot = {0,0,0};
	for (sint32 i = 0; i < 16 ; i++)
	{
		NewChild = MakeARope (ropeColl, 0, offset, 0);
		NewtonJoint *joint = NewtonConstraintCreateBall (nWorld, &pivot[0], NewChild, Parent);

		//dVector pin (location.m_up.Scale (-1.0f));
		vec3_t pin = {0,-1,0};
		NewtonBallSetConeLimits (joint, &pin[0], 0.0f, 10.0f * 3.1416f / 180.0f); 


		pivot[1] -= 8;
		offset -= 8;
	}

	Vec3Set (pivot, 0,0,0);
	NewtonConstraintCreateBall (nWorld, &pivot[0], b, NULL);*/

	inited = true;

	FILE *fp;
	string temp;

	temp = "baseq2/maps/";
	temp += level.ServerLevelName;
	temp += ".bspmesh";

	fp = fopen (temp.c_str(), "rb");

	if (!fp)
	{
		gi.dprintf ("WARNING: No BSPMESH! Using a plane for newton collision.\n");

		NewtonCollision *map = NewtonCreateTreeCollision(nWorld, NULL);
		NewtonTreeCollisionBeginBuild(map);

		// 4 vertices in this map
		vec3_t mapVerts[4] =
		{
			{2000,-270,2000},
			{2000,-270,-2000},
			{-2000,-270,-2000},
			{-2000,-270,2000}
		};

		NewtonTreeCollisionAddFace (map, 4, &mapVerts[0][0], 12, 1);
		NewtonTreeCollisionEndBuild(map, 1);
		NewtonBody *worldmap = NewtonCreateBody(nWorld, map);
	}
	else
	{
		//sint32 numSurfaces;

		NewtonCollision *map = NewtonCreateTreeCollision(nWorld, NULL);
		NewtonTreeCollisionBeginBuild(map);

		/*fread (&numSurfaces, sizeof(sint32), 1, fp);

		for (sint32 i = 0; i < numSurfaces; i++)
		{
			sint32 numIndexes;

			fread (&numIndexes, sizeof(sint32), 1, fp);

			vec3_t *indexArray = new vec3_t[numIndexes];
			for (sint32 z = 0; z < numIndexes; z++)
			{
				fread (&indexArray[z][0], sizeof(float), 1, fp);
				fread (&indexArray[z][1], sizeof(float), 1, fp);
				fread (&indexArray[z][2], sizeof(float), 1, fp);
			}
			NewtonTreeCollisionAddFace (map, numIndexes/3, &indexArray[0][0], 12, 1);
			delete[numIndexes] indexArray;
		}*/

		/*for (long i = 0; i < numVerts; i++)
		{
			fread (&read, sizeof(float), 3, fp);

			NewtonTreeCollisionAddFace (map, 1, &read[0],
											12, 0);
		}*/
		//NewtonTreeCollisionAddFace (map, numVerts, readVectors[0], sizeof(float) * 3, 1);

		sint32 numSurfaces;
		fread (&numSurfaces, sizeof(sint32), 1, fp);
		for (sint32 i = 0; i < numSurfaces; i++)
		{
			sint32 numVerts;
			vec3_t *vertexArray;
			sint32 numIndexes;
			index_t *indexArray;

			fread (&numVerts, sizeof(sint32), 1, fp);
			vertexArray = new vec3_t[numVerts];
			fread (vertexArray, sizeof(vec3_t), numVerts, fp);

			for (sint32 x = 0; x < numVerts; x++)
			{
				vec3_t newVec;
				Vec3Set (newVec, vertexArray[x][0], vertexArray[x][2], vertexArray[x][1]);
				Vec3Copy (newVec, vertexArray[x]);
			}

			fread (&numIndexes, sizeof(sint32), 1, fp);
			indexArray = new index_t[numIndexes];
			fread (indexArray, sizeof(index_t), numIndexes, fp);

			for (sint32 z = 0; z < (numIndexes / 3); z++)
			{
				vec3_t triangle[3];

				Vec3Copy (vertexArray[indexArray[(z * 3)]], triangle[0]);
				Vec3Copy (vertexArray[indexArray[(z * 3)+1]], triangle[1]);
				Vec3Copy (vertexArray[indexArray[(z * 3)+2]], triangle[2]);

				NewtonTreeCollisionAddFace (map, 3, &triangle[0][0], 12, 0);
			}

			delete[numVerts] vertexArray;
			delete[numIndexes] indexArray;
		}

		fclose(fp);

		NewtonTreeCollisionEndBuild(map, 1);
		NewtonBody *worldmap = NewtonCreateBody(nWorld, map);

	}

	TimeStart = GetTickCount();
}

void Cmd_NewtonInit (edict_t *ent)
{
	if (inited)
		return;
	InitNewton();
}

bool InitedRagdollCollisions = false;

// NOTE: WIDTH HEIGHT LENGTH!
NewtonCollision *BodyCollisions[10];

NewtonRagDollBone *QuickRagdoll (edict_t *player, NewtonRagDoll *Ragdoll, NewtonRagDollBone *Parent, float x, float y, float z, sint32 frame)
{
	float size = 4, mass = 24;
	switch (frame)
	{
	case 0:
		size = 12;
		mass = 90;
		break;
	case 1:
		size = 4;
		mass = 55;
		break;
	case 2:
	case 4:
		size = 3;
		mass = 25;
		break;
	case 3:
	case 5:
		size = 5;
		mass = 35;
		break;
	case 6:
	case 7:
	case 8:
	case 9:
		size = 3;
		mass = 20;
		break;
	}

	edict_t *ent = G_Spawn();
	ent->state.modelIndex = ModelIndex("models/ragdoll/tris.md2");
	ent->state.frame = frame;
	ent->owner = player;
	Vec3Set (ent->state.origin, x, y, z);
	Vec3Copy (ent->state.oldOrigin, ent->state.origin);
	gi.linkentity(ent);

	// Compile the 4x4 matrix.
	// I hate these.
	mat4x4_t mat;
	Matrix4_Identity (mat);
	Matrix4_Translate (mat, y, z, x);

	vec3_t sz = {size,size,size};

	NewtonRagDollBone *Bone = NewtonRagDollAddBone(Ragdoll, Parent, ent, mass, &mat[0], BodyCollisions[frame], &sz[0]);

	return Bone;
}

void RagDollForceCallBackOfDoom (const NewtonBody* body)
{
	float mass; 
	float Ixx; 
	float Iyy; 
	float Izz; 
	float force[3]; 
	float torque[3]; 

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz); 

	force[0] = 0.0f; 
	force[1] = -250 * mass; 
	force[2] = 0.0f; 

	torque[0] = 0.0f; 
	torque[1] = 0.0f; 
	torque[2] = 0.0f; 

	NewtonBodyAddForce (body, force); 
	NewtonBodyAddTorque (body, torque); 

	edict_t *applied = (edict_t*)NewtonRagDollBoneGetUserData((NewtonRagDollBone*)NewtonBodyGetUserData(body));
	if (applied->air_finished)
	{
		applied->air_finished = 0;

		vec3_t forward;

		Angles_Vectors (applied->owner->client->v_angle, forward, NULL, NULL);
		Vec3Scale (forward, 35 * mass, forward);

		float temp = forward[1];
		forward[1] = forward[2];
		forward[2] = temp;

		NewtonBodySetVelocity (body, &forward[0]);
	}
}

void RagDollTransCallBackOfDoom (const NewtonRagDollBone* bone)
{
	NewtonBody *body = NewtonRagDollBoneGetBody(bone);
	edict_t *attached = (edict_t*)NewtonRagDollBoneGetUserData (bone);
	// copy the matrix into an irrlicht matrix4
	mat4x4_t mat;
	NewtonBodyGetMatrix (body, &mat[0]);

	// Retreive the user data attached to the newton body
	/*ISceneNode *tmp = (ISceneNode *)NewtonBodyGetUserData(body);
	if (tmp)
	{
		// Position the node
		tmp->setPosition(mat.getTranslation());		// set position
		tmp->setRotation(mat.getRotationDegrees());	// and rotation
	}*/
	//gi.dprintf ("%f %f %f\n", mat[12], mat[13], mat[14]);
	attached->state.origin[0] = mat[12];
	attached->state.origin[2] = mat[13];
	attached->state.origin[1] = mat[14];

	NewtonBodyGetMatrix (body, &mat[0]);
	NewtonGetEulerAngle (&mat[0], &attached->state.angles[0]);
	attached->state.angles[0] = RAD2DEG(attached->state.angles[0]);
	attached->state.angles[1] = RAD2DEG(attached->state.angles[1]);
	attached->state.angles[2] = RAD2DEG(attached->state.angles[2]);
    // Assuming the angles are in radians.
	/*if (mat[10] > 0.998) { // singularity at north pole
		attached->state.angles[0] = atan2(mat[2],mat[22]);
		attached->state.angles[1] = M_PI/2;
		attached->state.angles[2] = 0;
	}
	else if (mat[10] < -0.998) { // singularity at south pole
		attached->state.angles[0] = atan2(mat[2],mat[22]);
		attached->state.angles[1] = -M_PI/2;
		attached->state.angles[2] = 0;
	}
	else
	{
		attached->state.angles[0] = atan2(-mat[20],mat[0]);
		attached->state.angles[1] = atan2(-mat[12],mat[11]);
		attached->state.angles[2] = asin(mat[10]);
	}
	attached->state.angles[0] = RAD2DEG(attached->state.angles[0]);
	attached->state.angles[1] = RAD2DEG(attached->state.angles[1]);
	attached->state.angles[2] = RAD2DEG(attached->state.angles[2]);*/

	gi.linkentity(attached);
}

void Cmd_NewtonBox (edict_t *ent)
{
	if (Q_stricmp(gi.argv(1), "fire") == 0)
	{
		vec3_t forward, end;

		Angles_Vectors (ent->client->v_angle, forward, NULL, NULL);
		Vec3MA (ent->state.origin, 8192, forward, end);

		CTrace trace;

		trace.Trace(ent->state.origin, end, ent, CONTENTS_MASK_SHOT);
		if (trace.ent && trace.ent->inUse && trace.ent->newtonBody)
		{
			float mass; 
			float Ixx; 
			float Iyy; 
			float Izz; 

			NewtonBodyGetMassMatrix (trace.ent->newtonBody, &mass, &Ixx, &Iyy, &Izz); 

			vec3_t angles;
			Vec3Copy (ent->client->v_angle, angles);
			Angles_Vectors (angles, forward, NULL, NULL);
			Vec3Scale (forward, 35 * mass, forward);

			float temp = forward[1];
			forward[1] = forward[2];
			forward[2] = temp;

			NewtonBodySetVelocity (trace.ent->newtonBody, &forward[0]);
		}

		return;
	}
	else if (Q_stricmp(gi.argv(1), "triangle") == 0)
	{
		vec3_t origin;

		Vec3Copy (ent->state.origin, origin);

		MakeAStupidBox (ent, glob, origin[0], origin[2], origin[1]);
		MakeAStupidBox (ent, glob, origin[0]+100, origin[2], origin[1]+50);
		MakeAStupidBox (ent, glob, origin[0]+100, origin[2], origin[1]-50);
		MakeAStupidBox (ent, glob, origin[0]+200, origin[2], origin[1]+100);
		MakeAStupidBox (ent, glob, origin[0]+200, origin[2], origin[1]);
		MakeAStupidBox (ent, glob, origin[0]+200, origin[2], origin[1]-100);
		MakeAStupidBox (ent, glob, origin[0]+300, origin[2], origin[1]+150);
		MakeAStupidBox (ent, glob, origin[0]+300, origin[2], origin[1]+50);
		MakeAStupidBox (ent, glob, origin[0]+300, origin[2], origin[1]-50);
		MakeAStupidBox (ent, glob, origin[0]+300, origin[2], origin[1]-150);

		return;
	}
	else if (Q_stricmp(gi.argv(1), "remove") == 0)
	{
		vec3_t forward, end;

		Angles_Vectors (ent->client->v_angle, forward, NULL, NULL);
		Vec3MA (ent->state.origin, 8192, forward, end);

		CTrace trace;

		trace.Trace(ent->state.origin, end, ent, CONTENTS_MASK_SHOT);
		if (trace.ent && trace.ent->inUse && trace.ent->newtonBody)
		{
			NewtonDestroyBody (nWorld, trace.ent->newtonBody);
			G_FreeEdict (trace.ent);
		}

		return;
	}
	else if (Q_stricmp(gi.argv(1), "ragdoll") == 0)
	{
/*		if (!InitedRagdollCollisions)
		{
			BodyCollisions[0] = NewtonCreateBox (nWorld, 14, 21, 13, NULL);
			BodyCollisions[1] = NewtonCreateCapsule (nWorld, 6, 5, NULL);
			BodyCollisions[2] = NewtonCreateBox (nWorld, 8, 16, 9, NULL);
			BodyCollisions[3] = NewtonCreateCapsule (nWorld, 8, 22, NULL);

			BodyCollisions[4] = BodyCollisions[2];// NewtonCreateBox (nWorld, 8, 16, 9, NULL);
			BodyCollisions[5] = BodyCollisions[3];// NewtonCreateCapsule (nWorld, 8, 22, NULL);

			BodyCollisions[6] = NewtonCreateBox (nWorld, 12, 6, 10, NULL);
			BodyCollisions[7] = BodyCollisions[6];// NewtonCreateBox (nWorld, 12, 6, 10, NULL);

			BodyCollisions[8] = BodyCollisions[6];// NewtonCreateBox (nWorld, 12, 6, 10, NULL);
			BodyCollisions[9] = BodyCollisions[7];// NewtonCreateBox (nWorld, 12, 6, 10, NULL);
		}

		NewtonRagDoll *NewRagdoll = HT (nWorld);
		NewtonRagDollBegin(NewRagdoll);

		// Add the bones
		// Body
		NewtonRagDollBone *Body = QuickRagdoll (ent, NewRagdoll, NULL, ent->state.origin[0], ent->state.origin[1], ent->state.origin[2], 0);
		QuickRagdoll (ent, NewRagdoll, Body, 0, 0, 7, 1);
		NewtonRagDollBone *LT = QuickRagdoll (ent, NewRagdoll, Body, 4, 0, -10, 2);
		QuickRagdoll (ent, NewRagdoll, LT, 4, 0, -8, 3);
		NewtonRagDollBone *RT = QuickRagdoll (ent, NewRagdoll, Body, -4, 0, -10, 4);
		QuickRagdoll (ent, NewRagdoll, RT, -4, 0, -8, 5);
		NewtonRagDollBone *LH = QuickRagdoll (ent, NewRagdoll, Body, 7, 0, 2, 6);
		QuickRagdoll (ent, NewRagdoll, LH, 7, 0, -8, 5);
		NewtonRagDollBone *RH = QuickRagdoll (ent, NewRagdoll, Body, -7, 0, 2, 6);
		QuickRagdoll (ent, NewRagdoll, RH, -7, 0, -8, 5);

		NewtonRagDollSetForceAndTorqueCallback  (NewRagdoll, RagDollForceCallBackOfDoom);
		NewtonRagDollSetTransformCallback (NewRagdoll, RagDollTransCallBackOfDoom);

		NewtonRagDollEnd(NewRagdoll);
		return;*/

		/*
		vec3_t origin = {0,120,0};

		NewtonBody *Attach = MakeARope(glob, 0, 120, 0);
		origin[1] += 0;

		NewtonJoint *Test = NewtonConstraintCreateBall (nWorld, &origin[0], Attach, NULL);

		origin[1] = 120 - 16;
		NewtonBody *AttachTo = MakeARope(glob, 0, origin[1], 0);
		origin[1] = 120 - 8;

		mat4x4_t matri;
		NewtonBodyGetMatrix (AttachTo, &matri[0]);

		mat3x3_t ma;
		Matrix4_Matrix3 (matri, ma);

		vec3_t angles;
		Matrix3_Angles (ma, angles);

		vec3_t up;
		Angles_Vectors (angles, NULL, NULL, up);
		up[0] *= -1;
		up[1] *= -1;
		up[2] *= -1;

		NewtonBallSetConeLimits (Test, &up[0], 0.0f, 10.0f * 3.1416f / 180.0f); 
		Test = NewtonConstraintCreateBall (nWorld, &origin[0], AttachTo, Attach);
		NewtonBallSetConeLimits (Test, &up[0], 0.0f, 10.0f * 3.1416f / 180.0f); 
		return;*/
	}

	MakeAStupidBox (ent, glob, ent->state.origin[0], ent->state.origin[2]+16, ent->state.origin[1]);
}

void ItFunc (const NewtonBody* body)
{

	edict_t *attached = (edict_t*)NewtonBodyGetUserData(body);

	if (!attached)
		return;

	/*NewtonBodyGetMatrix (body, &mat[0]);
	NewtonGetEulerAngle (&mat[0], &attached->state.angles[0]);
	attached->state.angles[0] = RAD2DEG(attached->state.angles[0]);
	attached->state.angles[1] = RAD2DEG(attached->state.angles[1]);
	attached->state.angles[2] = RAD2DEG(attached->state.angles[2]);*/
	attached->newtonBody = body;
}

void DrawNewton ()
{
	DWORD Sep;
	if (!inited)
		return;

	TimeCurrent = GetTickCount();

	Sep = (TimeCurrent - TimeStart);

	//gi.dprintf ("%u\n", Sep);

	float step = ((float)Sep / 100);

	for (sint32 i = 0; i < 9; i++)
		NewtonUpdate(nWorld, step);

	NewtonWorldForEachBodyDo(nWorld,ItFunc);

	TimeStart = GetTickCount();
}




/* MD2 header */
struct md2_header_t
{
  sint32 ident;
  sint32 version;

  sint32 skinwidth;
  sint32 skinheight;

  sint32 framesize;

  sint32 num_skins;
  sint32 num_vertices;
  sint32 num_st;
  sint32 num_tris;
  sint32 num_glcmds;
  sint32 num_frames;

  sint32 offset_skins;
  sint32 offset_st;
  sint32 offset_tris;
  sint32 offset_frames;
  sint32 offset_glcmds;
  sint32 offset_end;
};

/* Texture name */
struct md2_skin_t
{
  char name[64];
};

/* Texture coords */
struct md2_texCoord_t
{
  sint16 s;
  sint16 t;
};

/* Triangle info */
struct md2_triangle_t
{
  uint16 vertex[3];
  uint16 st[3];
};

/* Compressed vertex */
struct md2_vertex_t
{
  uint8 v[3];
  uint8 normalIndex;
};

/* Model frame */
struct md2_frame_t
{
  vec3_t scale;
  vec3_t translate;
  char name[16];
  struct md2_vertex_t *verts;
};

/* GL command packet */
struct md2_glcmd_t
{
  float s;
  float t;
  sint32 index;
};

/* MD2 model structure */
struct md2_model_t
{
  struct md2_header_t header;

  struct md2_skin_t *skins;
  struct md2_texCoord_t *texcoords;
  struct md2_triangle_t *triangles;
  struct md2_frame_t *frames;
  sint32 *glcmds;

  sint32 tex_id;
};

/*** An MD2 model ***/
struct md2_model_t md2file;


/**
 * Load an MD2 model from file.
 *
 * Note: MD2 format stores model's data in little-endian ordering.  On
 * big-endian machines, you'll have to perform proper conversions.
 */
sint32
ReadMD2Model (const char *filename, struct md2_model_t *mdl)
{
  FILE *fp;
  sint32 i;

  fp = fopen (filename, "rb");
  if (!fp)
    {
      fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
      return 0;
    }

  /* Read header */
  fread (&mdl->header, 1, sizeof (struct md2_header_t), fp);

  if ((mdl->header.ident != 844121161) ||
      (mdl->header.version != 8))
    {
      /* Error! */
      fprintf (stderr, "Error: bad version or identifier\n");
      fclose (fp);
      return 0;
    }

  /* Memory allocations */
  mdl->skins = (struct md2_skin_t *)
    malloc (sizeof (struct md2_skin_t) * mdl->header.num_skins);
  mdl->texcoords = (struct md2_texCoord_t *)
    malloc (sizeof (struct md2_texCoord_t) * mdl->header.num_st);
  mdl->triangles = (struct md2_triangle_t *)
    malloc (sizeof (struct md2_triangle_t) * mdl->header.num_tris);
  mdl->frames = (struct md2_frame_t *)
    malloc (sizeof (struct md2_frame_t) * mdl->header.num_frames);
  mdl->glcmds = (sint32 *)malloc (sizeof (sint32) * mdl->header.num_glcmds);

  /* Read model data */
  fseek (fp, mdl->header.offset_skins, SEEK_SET);
  fread (mdl->skins, sizeof (struct md2_skin_t),
	 mdl->header.num_skins, fp);

  fseek (fp, mdl->header.offset_st, SEEK_SET);
  fread (mdl->texcoords, sizeof (struct md2_texCoord_t),
	 mdl->header.num_st, fp);

  fseek (fp, mdl->header.offset_tris, SEEK_SET);
  fread (mdl->triangles, sizeof (struct md2_triangle_t),
	 mdl->header.num_tris, fp);

  fseek (fp, mdl->header.offset_glcmds, SEEK_SET);
  fread (mdl->glcmds, sizeof (sint32), mdl->header.num_glcmds, fp);

  /* Read frames */
  fseek (fp, mdl->header.offset_frames, SEEK_SET);
  for (i = 0; i < mdl->header.num_frames; ++i)
    {
      /* Memory allocation for vertices of this frame */
      mdl->frames[i].verts = (struct md2_vertex_t *)
	malloc (sizeof (struct md2_vertex_t) * mdl->header.num_vertices);

      /* Read frame data */
      fread (mdl->frames[i].scale, sizeof (vec3_t), 1, fp);
      fread (mdl->frames[i].translate, sizeof (vec3_t), 1, fp);
      fread (mdl->frames[i].name, sizeof (char), 16, fp);
      fread (mdl->frames[i].verts, sizeof (struct md2_vertex_t),
	     mdl->header.num_vertices, fp);
    }

  fclose (fp);
  return 1;
}

/**
 * Free resources allocated for the model.
 */
void
FreeModel (struct md2_model_t *mdl)
{
  sint32 i;

  if (mdl->skins)
    {
      free (mdl->skins);
      mdl->skins = NULL;
    }

  if (mdl->texcoords)
    {
      free (mdl->texcoords);
      mdl->texcoords = NULL;
    }

  if (mdl->triangles)
    {
      free (mdl->triangles);
      mdl->triangles = NULL;
    }

  if (mdl->glcmds)
    {
      free (mdl->glcmds);
      mdl->glcmds = NULL;
    }

  if (mdl->frames)
    {
      for (i = 0; i < mdl->header.num_frames; ++i)
	{
	  free (mdl->frames[i].verts);
	  mdl->frames[i].verts = NULL;
	}

      free (mdl->frames);
      mdl->frames = NULL;
    }
}

void Convert (const char *file)
{
	struct md2_model_t poop;

	if (!ReadMD2Model(file, &poop))
		return;

	string temp = file;

	temp = temp.substr (0, temp.length()-3);
	temp += "md2mesh";

	FILE *fp = fopen (temp.c_str(), "wb+");

	fwrite (&poop.header.num_tris, sizeof(sint32), 1, fp);
	for (sint32 i = 0; i < poop.header.num_tris; i++)
	{
		md2_vertex_t *verts = poop.frames[0].verts;
		md2_triangle_t *faces = poop.triangles;

		//fwrite (&faces[i].vertex, sizeof(sint16), 3, fp);

	    /* Calculate vertex real position */
		vec3_t v[3];

		for (sint32 j = 0; j < 3; ++j)
		{
			md2_frame_t *pframe = &poop.frames[0];
			md2_vertex_t *pvert = &pframe->verts[poop.triangles[i].vertex[j]];

			/* Calculate vertex real position */
			v[j][0] = (pframe->scale[0] * pvert->v[0]) + pframe->translate[0];
			v[j][2] = (pframe->scale[1] * pvert->v[1]) + pframe->translate[1];
			v[j][1] = (pframe->scale[2] * pvert->v[2]) + pframe->translate[2];
		}

		fwrite (v, sizeof(float) * 3, 3, fp);
	}

	fclose(fp);

	FreeModel (&poop);
}

typedef struct storage_s
{
	vec3_t	vectors[3];
} storage_t;

NewtonCollision *LoadInCollision (char *modelName)
{
	FILE *fp = fopen(modelName, "rb");

	if (!fp)
	{
		string Name = modelName;
		Name = Name.substr (0, Name.length()-7);
		Name += "md2";
		Convert (Name.c_str());
		fp = fopen(modelName, "rb");

		if (!fp)
			return NULL;
	}

	sint32 numVerts;
	fread (&numVerts, sizeof(sint32), 1, fp);

	mat3x3_t *vertices = new mat3x3_t[numVerts];

	bool pope = false;
	for (sint32 i = 0; i < numVerts; i++)
	{
		fread (&vertices[i][0][0], sizeof(float) * 3, 3, fp);

		
	}

	NewtonCollision *map = NewtonCreateConvexHull (nWorld, numVerts, &vertices[0][0][0], 12, NULL);
	delete[numVerts] vertices;

	return map;
}
#endif