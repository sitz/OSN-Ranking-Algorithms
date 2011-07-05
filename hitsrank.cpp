// HITS Rank Algorithm

#include "stdafx.h"

int main(int argc, char* argv[]) {
 Env = TEnv(argc, argv, TNotify::StdNotify);
 Env.PrepArgs(TStr::Fmt("HITS Rank. Build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
 TExeTm ExeTm;
 Try
	const TStr Iput = Env.GetIfArgPrefixStr("-i:", "Input.txt", "Input File");
	const TStr Oput = Env.GetIfArgPrefixStr("-o:", "Output.txt", "Output File");
	FILE* fpI = fopen(Iput.CStr(), "r");
	FILE* fpO = fopen(Oput.CStr(), "w");

	
	PNGraph Graph = TSnap::LoadEdgeList< PNGraph > (Iput);
	fprintf(fpO, "\nNodes: %d, Edges: %d\n\n", Graph->GetNodes(), Graph->GetEdges());
	
	const int NNodes  = Graph->GetNodes();
	const int MaxIter = 50;
	
	TIntFltH NIdHubH, NIdAuthH;
	NIdHubH.Gen(NNodes);
	NIdAuthH.Gen(NNodes);
	
	for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
		NIdHubH.AddDat(NI.GetId(), 1.0);
		NIdAuthH.AddDat(NI.GetId(), 1.0);
	}
	
	double Norm = 0;
	for (int iter = 0; iter < MaxIter; iter++) {
    
    // update authority scores
		Norm = 0.0;
		for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
			double& Auth = NIdAuthH.GetDat(NI.GetId()).Val;
			Auth = 0;
			for (int e = 0; e < NI.GetInDeg(); e++) {
				Auth +=  NIdHubH.GetDat(NI.GetInNId(e));
			}
			Norm += Auth*Auth;
		}
	    Norm = sqrt(Norm);
	    for (int i = 0; i < NIdAuthH.Len(); i++) {
	    	NIdAuthH[i] /= Norm;
		}

	// update hub scores
		Norm = 0.0;
		for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
			double& Hub = NIdHubH.GetDat(NI.GetId()).Val;
			Hub = 0; 
			for (int e = 0; e < NI.GetOutDeg(); e++) {
				Hub += NIdAuthH.GetDat(NI.GetOutNId(e));
			}
			Norm += Hub*Hub;
		}
		Norm = sqrt(Norm);
		for (int i = 0; i < NIdHubH.Len(); i++) {
			NIdHubH[i] /= Norm;
		}
	}
	
	fprintf(fpO, "Node ID\t\tAuthority Value\t\tHub Value\n");
	for (int i = 0; i < NNodes; i++){
		fprintf(fpO, "%d\t\t\t%.5lf\t\t\t\t%.5lf\n", i, (double) NIdAuthH[i], (double) NIdHubH[i]);
	}

 Catch
	printf("\nRun Time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());	

  return 0;
}
