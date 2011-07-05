//Inverse Trust Rank Algorithm

#include "stdafx.h"

int main(int argc, char* argv[]) {
 Env = TEnv(argc, argv, TNotify::StdNotify);
 Env.PrepArgs(TStr::Fmt("Trust Rank. Build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
 TExeTm ExeTm;
 Try
	const TStr Gnod = Env.GetIfArgPrefixStr("-g:", "Gnode.txt", "Good Nodes");
	const TStr Bnod = Env.GetIfArgPrefixStr("-b:", "Bnode.txt", "Bad Nodes" );
	const TStr Iput = Env.GetIfArgPrefixStr("-i:", "Input.txt", "Input File");
	const TStr Oput = Env.GetIfArgPrefixStr("-o:", "Output.txt", "Output File");
	const double C	  = 0.85;
	const int MaxIter = 50;
	const double Eps  = 1e-9;
	
	FILE* fpO = fopen(Oput.CStr(), "w");
	
	PNGraph Graph = TSnap::LoadEdgeList< PNGraph > (Iput);
	fprintf(fpO, "\nNodes: %d, Edges: %d\n\n", Graph->GetNodes(), Graph->GetEdges());
	const int NNodes = Graph->GetNodes();
	TIntFltH TRankH;
	TRankH.Gen(NNodes);
	int maxNId = 0, NId = 0, ret = 0;
	for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++)
		maxNId = max(maxNId, NI.GetId());
	
	TFltV initialTrustScore(maxNId + 1);
	for (int i = 0; i < initialTrustScore.Len(); i++)
		initialTrustScore[i] = 0.5;

	FILE* fpI = fopen(Gnod.CStr(), "r");
	while (true) {
		ret = fscanf(fpI, "%d", &NId);
		if (ret == EOF) break;
		if (Graph->IsNode(NId))
			initialTrustScore[NId] = 1.0;
	}
	fclose(fpI);
	
	fpI = fopen(Bnod.CStr(), "r");
	while (true) {
		ret = fscanf(fpI, "%d", &NId);
		if (ret == EOF) break;
		if (Graph->IsNode(NId))
			initialTrustScore[NId] = 0.0;
	}
	fclose(fpI);

	double Tot = 0.0;
	for(int i = 0; i < initialTrustScore.Len(); i++)
		Tot += initialTrustScore[i];
	for(int i = 0; i < initialTrustScore.Len(); i++)
		initialTrustScore[i] /= Tot;
	
	for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++)
		TRankH.AddDat( NI.GetId(), initialTrustScore[NI.GetId()] );
	
	TFltV TmpV(NNodes);
	for (int iter = 0; iter < MaxIter; iter++) {
		int j = 0;
		for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, j++) {
			TmpV[j] = 0;
			for (int e = 0; e < NI.GetOutDeg(); e++) {
				const int OutNId = NI.GetOutNId(e);
				const int InDeg  = Graph->GetNI(InNId).GetInDeg();
				if (InDeg > 0) 
					TmpV[j] += (double) TRankH.GetDat(OutNId) / (double) InDeg; 
        	}
			TmpV[j] =  C * TmpV[j] + (1.0 - C) * initialTrustScore[NI.GetId()]; 
		}

		for (int i = 0; i < TRankH.Len(); i++) 
			TRankH[i] = TmpV[i];
	}
	
	fprintf(fpO, "Node ID\t\tTrustRank\n");
	for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++){
		int Id = NI.GetId();
		double tr = TRankH.GetDat(Id);
		fprintf(fpO, "%d\t\t\t%.5lf\n", Id, tr);
	}
	fclose(fpO);
	
 Catch
	printf("\nRun Time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
	
	return 0;
}
