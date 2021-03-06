/*
 Copyright (c) 2013 David Hoksza, Peter Szepe

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "match_data.h"
#include "parallel.h"
#include "rotation.h"

//! The class which merges RNA structures
class FormAvg
{
friend class ParallelMakeBackup;
friend class ParallelFullQVect;
friend class ParallelPreprocessRNA;
friend class ParallelMerge;

public:
	//! The constructor takes as parameter the merging result of two RNA generated by SETTER algorithm.
	/*!
	\param data	The data generated at mmatching the RNAs by SETTER
	*/
	FormAvg(MatchData &data);
	//! Constructor
	/*!
	\param mean		The mean distance between GSSUs, the information is from Input
	\param data		The matching data
	\param cnt		Output reference, increases the counter
	\param result	Output parameter, used to return the precision of the merging process
	*/
	FormAvg(double mean, MatchData &data, int &cnt, double &result);

	//! Returns the merged RNA structure.
	cRNAStructure getResult();

private:
	//! The constructor calls this function to calculate the average structure.
	void Calc();

	//! converts GSSU of an RNA to a GSSU without RNA.
	/*!
		\param hp		Reference for input hairpin
		\param outHp	The resulting hairpin
	*/
	static void MakeHairpin(
			const cHairpin &hp,  
			cHairpin &outHp);
	//! Makes RNA from hairipn/GSSU
	/*!
		\param rna		The RNA continig the input hairpin
		\param hp		The input hairpin
		\param	idx		The index of the hairpin in RNA
		\param	outRNA	The RNA structure created from a hairpin
	*/
	static void MakeRNAFromHairpin(
			cRNAStructure &rna,
			cHairpin &hp,
			hairpinIdx idx,
			cRNAStructure &outRna);

	//! Rotates and translates the GSSU represented as RNA structure
	/*!
		\param rna			Referenc for RNA, which will be rotated
		\param hpQIdx		The index of GSSU in rnaQ
		\param hpDBIdx		The index of GSSU in rnaDB
		\param inputType	The input hairpin's type, derived from the type of RNA, from which it was created.
	*/
	void Rotate(
			cRNAStructure &rna,
			hairpinIdx hpQIdx,
			hairpinIdx hpDBIdx,
			InputType inputType);

	//! The translation used in methode Rotate.
	/*!
		\param	rna		The RNA which will be translated.
		\param rMatrix	The translation vector.
	*/
	void Translate(
			cRNAStructure &rna, 
			cRMSD3DCoord &tVector);

	//! Rotates the coordinates of atom in a RNA.
	/*!
		\param	rna		The RNA which will be rotated.
		\param rMatrix	The rotation matrix.
	*/
	void Rotate(
			cRNAStructure &rna, 
			RMatrix &rMatrix);

	//! Appends RNA structures with a new RNA structure.
	/*! It is used to joining the result of GSSU merging to the result of RNA merging.
		\param appendedRna	The RNA which will be appended.
		\param rna			The RNA which will be joined.
	*/
	void AppendRNA(
			cRNAStructure &appendedRna, 
			cRNAStructure &rna);
	
	//! Returns the  existing residue from the end of the GSSU.
	/*!
		\param hp	The input GSSU
		\param pos	The position of the returned residue counting from the end.
	*/
	tPairResiude GetLastResidue(cHairpin &hp, int pos);

	//! Returns the  existing residue from the begining of the GSSU.
	/*!
		\param hp	The input GSSU
		\param pos	The position of the returned residue counting from the begining.
	*/
	tPairResiude GetFirstResidue(cHairpin &hp, int pos);

	//! Swaps two RNA structures
	/*!
		Swaps two RNA structures derived from one GSSU in cases, if the length of
		actually processed part of rna2 in longer than for rna1. The mode variable describes the 
		actually processed part, which could be the r1 side of the stem, the head, the r2 side of the stem.
		<br>
		Let's consider an example, the mode is r2 side, the lengt of r2 side for rna1 is 4 and for rna2 is 6. 
		the two RNAs will be swapped, the result will be boolean. The length1 output parameter is the length of 
		the longer, length2 is the length of the shorter part.

		\param rna1		input rna, if the methode is terminated, it will be the longr
		\param rna2		input rna, if the methode is terminated, it will be the longr
		\param length1	the length of rna1
		\param length2	the length of rna2
		\mode		determines the actually processed part, it could be the r1 side of hairpin, head and r2 side of hairpin.
		\result			true, if the two RNAs were swapped.
	*/
	bool SwapRNA(
			cRNAStructure &rna1, 
			cRNAStructure &rna2, 
			int &length1, 
			int &length2, 
			int mode);

	//! Merges two RNA, calls AlternatingMergePairToRNA or PatternMergePairToRNA according the parameters of algorithm.
	/*!
		\param idx		The index of mereg GSSUs to mRnaQVectand mRnaDBVect.
		\param outRna	The result of merging process.
	*/
	void MergePairToRNA(
			unsigned idx, 
			cRNAStructure &outRna);

	//! Merges two RNA, the methode is described at the diploma thesis, at 3.2.3.2.3
	/*!
		\param idx		The index of mereg GSSUs to mRnaQVectand mRnaDBVect.
		\param outRna	The result of merging process.
	*/
	void AlternatingMergePairToRNA(
			unsigned idx, 
			cRNAStructure &outRna);

	//! Merges two RNA, the methode is described at the diploma thesis, at 3.2.3.2.3
	/*!
		\param idx		The index of mereg GSSUs to mRnaQVectand mRnaDBVect.
		\param outRna	The result of merging process.
	*/
	void PatternMergePairToRNA(
			unsigned idx, 
			cRNAStructure &outRna);

	//! Contains GSSUs from rnaQ which are constributed in lowest aggregated S distance - optimal GSSU for merging.
	RNAVect mRnaQVect;
	//! Contains GSSUs from rnaDB which are constributed in lowest aggregated S distance - optimal GSSU for merging.
	RNAVect mRnaDBVect;
	//! Data calculated by SETTER.
	MatchData &mData;
	//! The resulting RNA of the merging process
	cRNAStructure mResultRna;
	//! The mean distance between GSSUs, retrieved from Input.
	double mMean;
};

//! Intel TBB class, creates RNAs from each GSSU of the rnaQ.
class ParallelFullQVect
{
public:

	ParallelFullQVect(
		MatchData &data,
		RNAVect &fullQVect);

	void operator()(const tbb::blocked_range<size_t> &r) const;
	void serial(int size) const;

private:
	MatchData &mData;
	RNAVect &mFullQVect; 
};

//! Intel TBB class.
/*!
	Creats RNAs from the hairpins which are optimal for merging, the merges them.
*/
class ParallelMerge
{
public:
	ParallelMerge(
		FormAvg &aver,
		RNAVect &rnaQVect, 
		RNAVect &rnaDBVect,
		RNAVect &mergedRnaVect,
		vector<string> &output);

	void operator()(const tbb::blocked_range<size_t> &r) const;
	void serial(int size) const;

	void Print();

private:
	FormAvg &mAver;
	RNAVect &mRnaQVect; 
	RNAVect &mRnaDBVect;
	RNAVect  &mMergedRnaVect;
	vector<string> &mOutput;
	Parameters &param;
};

//! Intel TBB class, calculates the distances between the merged average RNA and the parent RNAs.
class ParallelNewRnaDis
{
public:
	ParallelNewRnaDis(
		cRNAStructure &newRna,
		cRNAStructure &rnaQ,
		cRNAStructure &rnaDB,
		vector<double> &dist);

	void operator()(const tbb::blocked_range<size_t> &r) const;
	void serial(int size) const;

private:
	cRNAStructure &mNewRna;
	cRNAStructure &mRnaQ;
	cRNAStructure &mRnaDB;
	vector<double> &mDist;
	
};
