/*--------------------------------------------------------------------------
 *--------------------------------------------------------------------------
 *
 * Copyright (C) 2008 The PECOS Development Team
 *
 * Please see http://pecos.ices.utexas.edu for more information.
 *
 * This file is part of the QUESO Library (Quantification of Uncertainty
 * for Estimation, Simulation and Optimization).
 *
 * QUESO is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QUESO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QUESO. If not, see <http://www.gnu.org/licenses/>.
 *
 *--------------------------------------------------------------------------
 *
 * $Id$
 *
 * Brief description of this file: 
 * 
 *--------------------------------------------------------------------------
 *-------------------------------------------------------------------------- */

#ifndef __UQ_CALIB_PROBLEM_H__
#define __UQ_CALIB_PROBLEM_H__

#include <uqMarkovChainSG1.h>
#include <uqInstantiateIntersection.h>
#include <uqVectorRV.h>
#include <uqScalarFunction.h>

#undef UQ_CALIB_PROBLEM_READS_SOLVER_OPTION

#define UQ_CALIB_PROBLEM_FILENAME_FOR_NO_OUTPUT_FILE "."

// _ODV = option default value
#define UQ_CALIB_PROBLEM_COMPUTE_SOLUTION_ODV 1
#define UQ_CALIB_PROBLEM_OUTPUT_FILE_NAME_ODV UQ_CALIB_PROBLEM_FILENAME_FOR_NO_OUTPUT_FILE
#ifdef UQ_CALIB_PROBLEM_READS_SOLVER_OPTION
#define UQ_CALIB_PROBLEM_SOLVER_ODV           "bayes_mc" // Bayesian formula + Markov Chain
#endif

template <class P_V,class P_M>
class uqStatisticalInverseProblemClass
{
public:
  uqStatisticalInverseProblemClass(const char*                               prefix,
                                   const uqBaseVectorRVClass      <P_V,P_M>& priorRv,
                                   const uqBaseScalarFunctionClass<P_V,P_M>& likelihoodFunction,
                                         uqGenericVectorRVClass   <P_V,P_M>& postRv);
 ~uqStatisticalInverseProblemClass();

        bool computeSolutionFlag      () const;
        void solveWithBayesMarkovChain(const P_V& initialValues,
                                       const P_M* proposalCovMatrix);
  const uqBaseVectorRVClass<P_V,P_M>&    priorRv() const;
  const uqGenericVectorRVClass<P_V,P_M>& postRv () const;

        void print                    (std::ostream& os) const;

private:
        void defineMyOptions          (po::options_description& optionsDesc);
        void getMyOptionValues        (po::options_description& optionsDesc);

  const uqBaseEnvironmentClass&              m_env;
        std::string                          m_prefix;

        po::options_description*             m_optionsDesc;
        std::string                          m_option_help;
	std::string                          m_option_computeSolution;
        std::string                          m_option_outputFileName;
#ifdef UQ_CALIB_PROBLEM_READS_SOLVER_OPTION
        std::string                          m_option_solver;
#endif

        bool                                 m_computeSolution;
        std::string                          m_outputFileName;
#ifdef UQ_CALIB_PROBLEM_READS_SOLVER_OPTION
	std::string                          m_solverString;
#endif

  const uqBaseVectorRVClass       <P_V,P_M>& m_priorRv;
  const uqBaseScalarFunctionClass <P_V,P_M>& m_likelihoodFunction;
        uqGenericVectorRVClass    <P_V,P_M>& m_postRv;

        uqVectorSetClass          <P_V,P_M>* m_solutionDomain;
        uqBaseVectorPdfClass      <P_V,P_M>* m_solutionPdf;
        uqBaseVectorMdfClass      <P_V,P_M>* m_solutionMdf;
        uqBaseVectorCdfClass      <P_V,P_M>* m_solutionCdf;
        uqBaseVectorRealizerClass <P_V,P_M>* m_solutionRealizer;

        uqMarkovChainSGClass      <P_V,P_M>* m_mcSeqGenerator;
        uqBaseVectorSequenceClass <P_V,P_M>* m_chain;
        uqArrayOfOneDGridsClass   <P_V,P_M>* m_mdfGrids;
        uqArrayOfOneDTablesClass  <P_V,P_M>* m_mdfValues;
};

template<class P_V,class P_M>
std::ostream& operator<<(std::ostream& os, const uqStatisticalInverseProblemClass<P_V,P_M>& obj);

template <class P_V,class P_M>
uqStatisticalInverseProblemClass<P_V,P_M>::uqStatisticalInverseProblemClass(
  const char*                               prefix,
  const uqBaseVectorRVClass      <P_V,P_M>& priorRv,
  const uqBaseScalarFunctionClass<P_V,P_M>& likelihoodFunction,
        uqGenericVectorRVClass   <P_V,P_M>& postRv)
  :
  m_env                   (priorRv.env()),
  m_prefix                ((std::string)(prefix) + "ip_"),
  m_optionsDesc           (new po::options_description("UQ Calibration Problem")),
  m_option_help           (m_prefix + "help"           ),
  m_option_computeSolution(m_prefix + "computeSolution"),
  m_option_outputFileName (m_prefix + "outputFileName" ),
#ifdef UQ_CALIB_PROBLEM_READS_SOLVER_OPTION
  m_option_solver         (m_prefix + "solver"),
#endif
  m_computeSolution       (UQ_CALIB_PROBLEM_COMPUTE_SOLUTION_ODV),
  m_outputFileName        (UQ_CALIB_PROBLEM_OUTPUT_FILE_NAME_ODV),
#ifdef UQ_CALIB_PROBLEM_READS_SOLVER_OPTION
  m_solverString          (UQ_CALIB_PROBLEM_SOLVER_ODV),
#endif
  m_priorRv               (priorRv),
  m_likelihoodFunction    (likelihoodFunction),
  m_postRv                (postRv),
  m_solutionDomain        (NULL),
  m_solutionPdf           (NULL),
  m_solutionMdf           (NULL),
  m_solutionCdf           (NULL),
  m_solutionRealizer      (NULL),
  m_mcSeqGenerator        (NULL),
  m_chain                 (NULL)
{
  if (m_env.subScreenFile()) {
    *m_env.subScreenFile() << "Entering uqStatisticalInverseProblemClass<P_V,P_M>::constructor()"
                           << ": prefix = "              << m_prefix
                           << std::endl;
  }

  defineMyOptions                (*m_optionsDesc);
  m_env.scanInputFileForMyOptions(*m_optionsDesc);
  getMyOptionValues              (*m_optionsDesc);

  if (m_env.subScreenFile()) {
    *m_env.subScreenFile() << "In uqStatisticalInverseProblemClass<P_V,P_M>::constructor()"
                           << ": after getting values of options, state of object is:"
                           << "\n" << *this
                           << std::endl;
  }

  if (m_env.subScreenFile()) {
    *m_env.subScreenFile() << "Leaving uqStatisticalInverseProblemClass<P_V,P_M>::constructor()"
                           << ": prefix = "              << m_prefix
                           << std::endl;
  }

  return;
}

template <class P_V,class P_M>
uqStatisticalInverseProblemClass<P_V,P_M>::~uqStatisticalInverseProblemClass()
{
  if (m_chain) {
    m_chain->clear();
    delete m_chain;
  }
  if (m_mcSeqGenerator  ) delete m_mcSeqGenerator;
  if (m_solutionRealizer) delete m_solutionRealizer;
  if (m_solutionCdf     ) delete m_solutionCdf;
  if (m_solutionMdf     ) delete m_solutionMdf;
  if (m_solutionPdf     ) delete m_solutionPdf;
  if (m_solutionDomain  ) delete m_solutionDomain;
  if (m_optionsDesc     ) delete m_optionsDesc;
}

template<class P_V,class P_M>
void
uqStatisticalInverseProblemClass<P_V,P_M>::defineMyOptions(
  po::options_description& optionsDesc)
{
  optionsDesc.add_options()
    (m_option_help.c_str(),                                                                                            "produce help message for calibration problem")
    (m_option_computeSolution.c_str(), po::value<bool       >()->default_value(UQ_CALIB_PROBLEM_COMPUTE_SOLUTION_ODV), "compute solution process"                    )
    (m_option_outputFileName.c_str(),  po::value<std::string>()->default_value(UQ_CALIB_PROBLEM_OUTPUT_FILE_NAME_ODV), "name of output file"                         )
#ifdef UQ_CALIB_PROBLEM_READS_SOLVER_OPTION
    (m_option_solver.c_str(),          po::value<std::string>()->default_value(UQ_CALIB_PROBLEM_SOLVER_ODV          ), "algorithm for calibration"                   )
#endif
  ;

  return;
}

template<class P_V,class P_M>
void
  uqStatisticalInverseProblemClass<P_V,P_M>::getMyOptionValues(
  po::options_description& optionsDesc)
{
  if (m_env.allOptionsMap().count(m_option_help.c_str())) {
    if (m_env.subScreenFile()) {
      *m_env.subScreenFile() << optionsDesc
                             << std::endl;
    }
  }

  if (m_env.allOptionsMap().count(m_option_computeSolution.c_str())) {
    m_computeSolution = m_env.allOptionsMap()[m_option_computeSolution.c_str()].as<bool>();
  }

  if (m_env.allOptionsMap().count(m_option_outputFileName.c_str())) {
    m_outputFileName = m_env.allOptionsMap()[m_option_outputFileName.c_str()].as<std::string>();
  }

#ifdef UQ_CALIB_PROBLEM_READS_SOLVER_OPTION
  if (m_env.allOptionsMap().count(m_option_solver.c_str())) {
    m_solverString = m_env.allOptionsMap()[m_option_solver.c_str()].as<std::string>();
  }
#endif

  return;
}

template <class P_V,class P_M>
bool
uqStatisticalInverseProblemClass<P_V,P_M>::computeSolutionFlag() const
{
  return m_computeSolution;
}

template <class P_V,class P_M>
void
uqStatisticalInverseProblemClass<P_V,P_M>::solveWithBayesMarkovChain(
  const P_V& initialValues,
  const P_M* proposalCovMatrix)
{
  m_env.fullComm().Barrier();
  //m_env.syncPrintDebugMsg("Entering uqStatisticalInverseProblemClass<P_V,P_M>::solveWithBayesMarkovChain()",3000000,m_env.fullComm());

  if (m_computeSolution == false) {
    if ((m_env.subScreenFile())) {
      *m_env.subScreenFile() << "In uqStatisticalInverseProblemClass<P_V,P_M>::solveWithBayesMarkovChain()"
                             << ": avoiding solution, as requested by user"
                             << std::endl;
    }
    return;
  }
  if ((m_env.subScreenFile())) {
    *m_env.subScreenFile() << "In uqStatisticalInverseProblemClass<P_V,P_M>::solveWithBayesMarkovChain()"
                           << ": computing solution, as requested by user"
                           << std::endl;
  }

  if (m_mcSeqGenerator  ) delete m_mcSeqGenerator;
  if (m_solutionRealizer) delete m_solutionRealizer;
  if (m_solutionCdf     ) delete m_solutionCdf;
  if (m_solutionMdf     ) delete m_solutionMdf;
  if (m_solutionPdf     ) delete m_solutionPdf;
  if (m_solutionDomain  ) delete m_solutionDomain;

  // Compute output pdf up to a multiplicative constant: Bayesian approach
  m_solutionDomain = uqInstantiateIntersection(m_priorRv.pdf().domainSet(),m_likelihoodFunction.domainSet());

  m_solutionPdf = new uqBayesianVectorPdfClass<P_V,P_M>(m_prefix.c_str(),
                                                        m_priorRv.pdf(),
                                                        m_likelihoodFunction,
                                                        *m_solutionDomain);
  m_postRv.setPdf(*m_solutionPdf);

  // Compute output realizer: Markov Chain approach
  m_chain = new uqSequenceOfVectorsClass<P_V,P_M>(m_postRv.imageSet().vectorSpace(),0,m_prefix+"chain");
  m_mcSeqGenerator = new uqMarkovChainSGClass<P_V,P_M>(m_prefix.c_str(),
                                                       m_postRv,
                                                       initialValues,
                                                       proposalCovMatrix);
  m_mcSeqGenerator->generateSequence(*m_chain);
  m_solutionRealizer = new uqSequentialVectorRealizerClass<P_V,P_M>(m_prefix.c_str(),
                                                                   *m_chain);
  m_postRv.setRealizer(*m_solutionRealizer);

  //m_env.syncPrintDebugMsg("In uqStatisticalInverseProblemClass<P_V,P_M>::solveWithBayesMarkovChain(), code place 1",3000000,m_env.fullComm());
  //m_env.fullComm().Barrier();

  // Compute output mdf: uniform sampling approach
  m_mdfGrids  = new uqArrayOfOneDGridsClass <P_V,P_M>((m_prefix+"mdf_").c_str(),m_postRv.imageSet().vectorSpace());
  m_mdfValues = new uqArrayOfOneDTablesClass<P_V,P_M>((m_prefix+"mdf_").c_str(),m_postRv.imageSet().vectorSpace());
  m_chain->uniformlySampledMdf(250,           // input
                               *m_mdfGrids,   // output
                               *m_mdfValues); // output
  m_solutionMdf = new uqSampledVectorMdfClass<P_V,P_M>(m_prefix.c_str(),
                                                       *m_mdfGrids,
                                                       *m_mdfValues);
  m_postRv.setMdf(*m_solutionMdf);

  if (m_outputFileName != UQ_CALIB_PROBLEM_FILENAME_FOR_NO_OUTPUT_FILE) {
    if (m_env.subRank() == 0) {
      // Write output file
      if (m_env.subScreenFile()) {
        *m_env.subScreenFile() << "Opening output file '" << m_outputFileName
                               << "' for calibration problem with problem with prefix = " << m_prefix
                               << std::endl;
      }

      // Open file
      std::ofstream* ofsvar = new std::ofstream((m_outputFileName+"_subenv"+m_env.subIdString()+".m").c_str(), std::ofstream::out | std::ofstream::in | std::ofstream::ate);
      if ((ofsvar            == NULL ) ||
          (ofsvar->is_open() == false)) {
        delete ofsvar;
        ofsvar = new std::ofstream((m_outputFileName+"_subenv"+m_env.subIdString()+".m").c_str(), std::ofstream::out | std::ofstream::trunc);
      }
      UQ_FATAL_TEST_MACRO((ofsvar && ofsvar->is_open()) == false,
                          m_env.rank(),
                          "uqStatisticalInverseProblem<P_V,P_M>::solveWithBayesMarkovChain()",
                          "failed to open file");

      m_postRv.mdf().print(*ofsvar);

      // Close file
      ofsvar->close();
      delete ofsvar;
      if (m_env.subScreenFile()) {
        *m_env.subScreenFile() << "Closed output file '" << m_outputFileName
                               << "' for calibration problem with problem with prefix = " << m_prefix
                               << std::endl;
      }
    }
  }
  if (m_env.subScreenFile()) {
    *m_env.subScreenFile() << std::endl;
  }

  //m_env.syncPrintDebugMsg("Leaving uqStatisticalInverseProblemClass<P_V,P_M>::solveWithBayesMarkovChain()",3000000,m_env.fullComm());
  m_env.fullComm().Barrier();
  
  return;
}

template <class P_V,class P_M>
const uqBaseVectorRVClass<P_V,P_M>& 
uqStatisticalInverseProblemClass<P_V,P_M>::priorRv() const
{
  return m_priorRv;
}

template <class P_V,class P_M>
const uqGenericVectorRVClass<P_V,P_M>& 
uqStatisticalInverseProblemClass<P_V,P_M>::postRv() const
{
  return m_postRv;
}

template <class P_V,class P_M>
void
uqStatisticalInverseProblemClass<P_V,P_M>::print(std::ostream& os) const
{
  os << "\n" << m_option_computeSolution << " = " << m_computeSolution
     << "\n" << m_option_outputFileName  << " = " << m_outputFileName
#ifdef UQ_CALIB_PROBLEM_READS_SOLVER_OPTION
     << "\n" << m_option_solver          << " = " << m_solverString
#endif
     << std::endl;
}

template<class P_V,class P_M>
std::ostream& operator<<(std::ostream& os, const uqStatisticalInverseProblemClass<P_V,P_M>& obj)
{
  obj.print(os);

  return os;
}
#endif // __UQ_CALIB_PROBLEM_H__
