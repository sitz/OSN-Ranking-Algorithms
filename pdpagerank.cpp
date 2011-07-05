// Paradoxial Discounted Page Rank

#include "stdafx.h"

int main(int argc, char* argv[]) {
 Env = TEnv(argc, argv, TNotify::StdNotify);
 Env.PrepArgs(TStr::Fmt("Paradioxial Discounted PageRank. Build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
 TExeTm ExeTm;
 Try
	const TStr Iput  = Env.GetIfArgPrefixStr("-i:", "Input.txt", "Input File" );
	const TStr Oput = Env.GetIfArgPrefixStr("-o:", "Output.txt", "Output File");
	FILE* fpI = fopen(Iput.CStr(), "r");
	FILE* fpO = fopen(Oput.CStr(), "w");

	
	const double C	= 0.85;
	const int MaxIter = 50;
	const double Eps = 1e-9;

	PNGraph Graph = TSnap::LoadEdgeList< PNGraph > (Iput);
	fprintf(fpO, "\nNodes: %d, Edges: %d\n\n", Graph->GetNodes(), Graph->GetEdges());
	const int NNodes = Graph->GetNodes();
	const double OneOver = (double) 1.0 / (double) NNodes;
	
	TIntFltH PRankH, PDiscountH;
	PRankH.Gen(NNodes); 
	PDiscountH.Gen(NNodes);		// this will store the paradoxical discount scores
	double MaxPD = 0;
	

	// compute paradoxical_discounted for all nodes
	for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++)
	{
		int Id = NI.GetId();
	    	int OutDeg = NI.GetOutDeg();
	    	int InDeg  = NI.GetInDeg();
	    	PRankH.AddDat(Id, OneOver);
    	
	    	if (InDeg > OutDeg)
		{
	    		double ratio = double(InDeg) / double(OutDeg);
	    		MaxPD = max(ratio, MaxPD);
	    		PDiscountH.AddDat(Id, ratio);
	    	}
	    	else 
		{
	    		int Rec  = 0;
	    		for (int e = 0; e < OutDeg; e++)
			{
	    			int OutNId = NI.GetOutNId(e);
	    			if (NI.IsInNId(OutNId))
	    				++Rec;
	    		}

	    		double ratio = double(InDeg - Rec) / double(OutDeg - Rec);
	    		MaxPD = max(ratio, MaxPD);
	    		PDiscountH.AddDat(Id, ratio);
	    	}
	}

	TFltV TmpV(NNodes);

	for (int iter = 1; iter < MaxIter; iter++)
	{
	    	int j = 0;
	    	for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, j++)
		{
			TmpV[j] = 0;
			for (int e = 0; e < NI.GetInDeg(); e++)
			{
				const int InNId = NI.GetInNId(e);
				const int OutDeg = Graph->GetNI(InNId).GetOutDeg();
				if (OutDeg > 0)
				{
					double ratio = (double) PRankH.GetDat(InNId) / (double) OutDeg;
					ratio = ratio * PDiscountH.GetDat(InNId) / MaxPD;
					TmpV[j] += ratio;
				}
			}
	      		TmpV[j] =  C * TmpV[j];
		}

		double diff = 0, sum = 0, NewVal;
		for (int i = 0; i < TmpV.Len(); i++)
			sum += TmpV[i];

		const double Leaked = (double) (1.0 - sum) / (double) NNodes;
		for (int i = 0; i < PRankH.Len(); i++) 
		{
			NewVal = TmpV[i] + Leaked;
			diff += fabs(NewVal - PRankH[i]);
			PRankH[i] = NewVal;
		}
		if (diff < Eps)
			break;
	}	// end for each iteration
	
	fprintf(fpO, "Node ID\t\tParadox PageRank\n");
	for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++){
		int Id = NI.GetId();
		const double pr = PRankH.GetDat(Id);
		fprintf(fpO, "%d\t\t\t%.5f\n", Id, pr);
	}
 Catch
	printf("\nRun Time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());	
	
	return 0;
}
