#include "BoundingBoxClass.h"

//  BoundingBoxClass
//The big 3
BoundingBoxClass::BoundingBoxClass(){Init();}
BoundingBoxClass::BoundingBoxClass(BoundingBoxClass const& other)
{
	m_bInitialized = other.m_bInitialized;
	m_v3Centroid = other.m_v3Centroid;
	m_sName = other.m_sName;
}
BoundingBoxClass& BoundingBoxClass::operator=(BoundingBoxClass const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		BoundingBoxClass temp(other);
		Swap(temp);
	}
	return *this;
}
BoundingBoxClass::~BoundingBoxClass(){Release();};

void BoundingBoxClass::Init(void)
{
	m_bInitialized = false;
	m_v3Centroid = vector3(0.0f);
	m_sName = "NULL";
}
void BoundingBoxClass::Swap(BoundingBoxClass& other)
{
	std::swap(m_bInitialized, other.m_bInitialized);
	std::swap(m_v3Centroid, other.m_v3Centroid);
	std::swap(m_sName, other.m_sName);
}
void BoundingBoxClass::Release(void)
{
	//No pointers to release
}

//Accessors
bool BoundingBoxClass::IsInitialized(void){ return m_bInitialized; }
vector3 BoundingBoxClass::GetCentroid(void){ return m_v3Centroid; }
String BoundingBoxClass::GetName(void){return m_sName;}


//Methods
void BoundingBoxClass::GenerateBoundingBox(String a_sInstanceName)
{
	//If this has already been initialized there is nothing to do here
	if(m_bInitialized)
		return;
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	if(pMeshMngr->IsInstanceCreated(a_sInstanceName))
	{
		m_sName = a_sInstanceName;
		
		std::vector<vector3> lVertices = pMeshMngr->GetVertices(m_sName);
		unsigned int nVertices = lVertices.size();
		m_v3Centroid = lVertices[0];
		v3Max = lVertices[0];
		v3Min = lVertices[0];
		for(unsigned int nVertex = 1; nVertex < nVertices; nVertex++)
		{
			if(v3Min.x > lVertices[nVertex].x)
				v3Min.x = lVertices[nVertex].x;
			else if(v3Max.x < lVertices[nVertex].x)
				v3Max.x = lVertices[nVertex].x;
			
			if(v3Min.y > lVertices[nVertex].y)
				v3Min.y = lVertices[nVertex].y;
			else if(v3Max.y < lVertices[nVertex].y)
				v3Max.y = lVertices[nVertex].y;

			if(v3Min.z > lVertices[nVertex].z)
				v3Min.z = lVertices[nVertex].z;
			else if(v3Max.z < lVertices[nVertex].z)
				v3Max.z = lVertices[nVertex].z;
		}
		m_v3Centroid = (v3Min + v3Max) / 2.0f;

		//Find the distance between the middle x,y,z and the vertices x,y,z
		fWidthX = glm::distance(m_v3Centroid.x, lVertices[0].x);
		fWidthY = glm::distance(m_v3Centroid.y, lVertices[0].y);
		fWidthZ = glm::distance(m_v3Centroid.z, lVertices[0].z);

		//Loop through all vertices and find the greatest distance
		for(unsigned int nVertex = 1; nVertex < nVertices; nVertex++){
			//Find the max X distance
			float fDistanceX = glm::distance(m_v3Centroid.x, lVertices[nVertex].x);
			if(fWidthX < fDistanceX)
				fWidthX = fDistanceX;
			//Find the max Y distance
			float fDistanceY = glm::distance(m_v3Centroid.y, lVertices[nVertex].y);
			if(fWidthY < fDistanceY)
				fWidthY = fDistanceY;
			//Find the max Z distance
			float fDistanceZ = glm::distance(m_v3Centroid.z, lVertices[nVertex].z);
			if(fWidthZ < fDistanceZ)
				fWidthZ = fDistanceZ;
		}

		m_bInitialized = true;
	}
}
void BoundingBoxClass::AddBoxToRenderList(matrix4 a_mModelToWorld, vector3 a_vColor, bool a_bRenderCentroid)
{
	if(!m_bInitialized)
		return;
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	if(a_bRenderCentroid)
		pMeshMngr->AddAxisToQueue(a_mModelToWorld * glm::translate(m_v3Centroid));

	//Setup matrix with all the info
	matrix4 cubeMatrix = a_mModelToWorld * glm::translate(m_v3Centroid) * glm::scale(vector3(fWidthX*2.0f,fWidthY*2.0f,fWidthZ*2.0f));
	pMeshMngr->AddCubeToQueue(cubeMatrix, a_vColor, MERENDER::WIRE);
}
