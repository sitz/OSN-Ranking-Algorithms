// Page Rank Algorithm

#include "stdafx.h"

int main(int argc, char* argv[]) {
 Env = TEnv(argc, argv, TNotify::StdNotify);
 Env.PrepArgs(TStr::Fmt("PageRank. Build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
 TExeTm ExeTm;
 Try
	const TStr Iput = Env.GetIfArgPrefixStr("-i:", "Input.txt", "Input File" );
	const TStr Oput = Env.GetIfArgPrefixStr("-o:", "Output.txt", "Output File");
	FILE* fpI = fopen(Iput.CStr(), "r");
	FILE* fpO = fopen(Oput.CStr(), "w");


	const double C	  = 0.85;
	const int MaxIter = 50;
	const double Eps  = 1e-9;

	PNGraph Graph = TSnap::LoadEdgeList< PNGraph > (Iput);
	fprintf(fpO, "\nNodes: %d, Edges: %d\n\n", Graph->GetNodes(), Graph->GetEdges());
	const int NNodes = Graph->GetNodes();
	const double OneOver = (double) 1.0 / (double) NNodes;
	
	TIntFltH PRankH;
	PRankH.Gen(NNodes);
	
	for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++)
    	PRankH.AddDat(NI.GetId(), OneOver);

	TFltV TmpV(NNodes);
	for (int iter = 0; iter < MaxIter; iter++) {
    	int j = 0;
    	for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, j++) {
			TmpV[j] = 0;
			for (int e = 0; e < NI.GetInDeg(); e++) {
				const int InNId = NI.GetInNId(e);
				const int OutDeg = Graph->GetNI(InNId).GetOutDeg();
				if (OutDeg > 0)
					TmpV[j] += (double) PRankH.GetDat(InNId) / (double) OutDeg;
			}
      		TmpV[j] =  C * TmpV[j];
		}
		double diff = 0, sum = 0, NewVal;
		for (int i = 0; i < TmpV.Len(); i++)
			sum += TmpV[i];

		const double Leaked = (double) (1.0 - sum) / (double) NNodes;
		for (int i = 0; i < PRankH.Len(); i++) {
			NewVal = TmpV[i] + Leaked;
			diff += fabs(NewVal - PRankH[i]);
			PRankH[i] = NewVal;
		}
		if (diff < Eps)
			break;
	}
	
	fprintf(fpO, "Node ID\t\tPageRank\n");
	for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++){
		int Id = NI.GetId();
		double pr = PRankH.GetDat(Id);
		fprintf(fpO, "%d\t\t\t%.5f\n", Id, pr);
	}
 Catch
	printf("\nRun Time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());	
	
	return 0;
}
