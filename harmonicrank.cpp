// Harmonic Rank Algorithm

#include "stdafx.h"

const double alpha = 0.85;

void Proximity(TFltV& HRank, PNGraph& Graph, TIntH& IDSMap, TIntH& Hash, int nodeR, int MaxIter){
	while (MaxIter--){
		for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++){
			int Id	= NI.GetId();
			int odU	= NI.GetOutDeg();
			int u	= IDSMap[Id];
			int H	= Hash[Id];
			if (H == 1) continue;	// this node is a member of the base-set or s or r

			double G = (double) (1.0 - alpha) / (double) odU;
			double V = 0.0;
			for (int e = 0; e < NI.GetOutDeg(); e++){
				int OutId = NI.GetOutNId(e);
				int v	  = IDSMap[OutId];
				double F = 0.0;
				if (v == nodeR)		F = alpha;
				else			F = G;
				V += F * HRank[v];
			}
			HRank[u]  = V;
		}
	}
}

int main(int argc, char* argv[]) {
 Env = TEnv(argc, argv, TNotify::StdNotify);
 Env.PrepArgs(TStr::Fmt("Harmonic Rank. Build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
 TExeTm ExeTm;
 Try
 	const TStr Iset = Env.GetIfArgPrefixStr("-s:", "Bset.txt", "Base Set File");
	const TStr Iput = Env.GetIfArgPrefixStr("-i:", "Input.txt", "Input File"  );
	const TStr Oput = Env.GetIfArgPrefixStr("-o:", "Output.txt", "Output File");
	
	PNGraph Graph = TSnap::LoadEdgeList< PNGraph > (Iput);
	FILE* fpI = fopen(Iset.CStr(), "r");
	FILE* fpO = fopen(Oput.CStr(), "w");
	
	int maxId = 0;
	for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) 
		maxId = max(maxId, NI.GetId());

	const int nodeS = maxId + 1;
	const int nodeR = maxId + 2;
 	Graph->AddNode(nodeS);
 	Graph->AddNode(nodeR);
 	
 	TIntV BaseSet;
 	TIntV NodeList;
 	
 	while (true){
 		int Id;
 		int flag = fscanf(fpI, "%d", &Id);
 		if (flag == EOF)
 			break;
 		BaseSet.AddUnique(Id);
 	}
	
	// add extra edges according to the algorithm (all nodes with outdegree 0 will link to r, others will link to r with some probability)
 	for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++){
 		int Id = NI.GetId();
 		NodeList.Add(Id);
 		if (Id == nodeS || Id == nodeR)
 			continue;

 		int inDeg  = NI.GetInDeg();
 		int OutDeg = NI.GetOutDeg();
 		if (OutDeg == 0)
 			Graph->AddEdge(Id, nodeR);
		else
		{
	 		double Rnd = (double) rand() / (double) RAND_MAX;
	 		if (Rnd >= alpha)
	 			Graph->AddEdge(Id, nodeR);
		}
 	}

	/*
	* NodeList contains nodes in the order in which they are accessed by the iterator
	* in other words, NodeList[i] is the i-th node in the order in which the iterator NI accesses nodes
	* But the ID of NodeList[i] may be different from i

	* SIDMap[i] gives the ID of NodeList[i]
	* IDSMap[i] gives the index of the node with ID i in the array NodeList

	* Hash[i] is 1 if the node with id NodeList[i] is in the base-set S, or is the node s or r

	* HRank[i] is the harmonic rank (as estimated up to now) of NodeList[i]
	* BaseSet stores the IDs of the nodes in the set S, and nodes s and r

	*/
 	
	// add extra edges according to the algorithm (all nodes in the base-set links to s)
 	for (int i = 0; i < BaseSet.Len(); i++)
 		Graph->AddEdge(BaseSet[i], nodeS);
	
	BaseSet.Add(nodeR);
	BaseSet.Add(nodeS);
 	int NodeCnt = Graph->GetNodes();
 	
 	TIntH SIDMap, IDSMap, Hash;
 	SIDMap.Gen(NodeCnt); IDSMap.Gen(NodeCnt); Hash.Gen(NodeCnt);
 	TFltVV F(NodeCnt, NodeCnt);
 	TFltV  HRank(NodeCnt);
 	
 	for (int i = 0; i < NodeCnt; i++){
 		IDSMap.AddDat(NodeList[i], i);
 		SIDMap.AddDat(i, NodeList[i]);
 		Hash.AddDat(NodeList[i], -1);		// initialize
 		HRank[i] = 0.0;
 	}
 	for (int i = 0; i < BaseSet.Len(); i++){
 		Hash[BaseSet[i]] = 1;
		int Id = BaseSet[i];
		if (Id == nodeR)
			HRank[IDSMap[Id]] = 0.0;
		else
			HRank[IDSMap[Id]] = 1.0;
	}
 		
 	Proximity(HRank, Graph, IDSMap, Hash, nodeR, 50);
 	
 	fprintf(fpO, "\nNodes: %d, Edges: %d\n", Graph->GetNodes(), Graph->GetEdges());
 	fprintf(fpO, "\nNode ID\t\tProximity Score\n");
 	for (int i = 0; i < NodeCnt; i++){
 		fprintf(fpO, "%d\t\t\t%.5lf\n", (int) NodeList[i], (double) HRank[i]);
 	}
 	
 	fclose(fpI);
 	fclose(fpO);
 
 Catch
	printf("\nRun Time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
	
	return 0;
}
