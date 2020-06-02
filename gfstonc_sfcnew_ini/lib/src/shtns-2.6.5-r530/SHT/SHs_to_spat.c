// This file was automatically generated by 'make' from file 'hyb_SH_to_spat.gen.c'.
// To modify it, please consider modifying hyb_SH_to_spat.gen.c
/*
 * Copyright (c) 2010-2015 Centre National de la Recherche Scientifique.
 * written by Nathanael Schaeffer (CNRS, ISTerre, Grenoble, France).
 * 
 * nathanael.schaeffer@ujf-grenoble.fr
 * 
 * This software is governed by the CeCILL license under French law and
 * abiding by the rules of distribution of free software. You can use,
 * modify and/or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 * 
 */


/////////////////////////////////////////////////////
  #ifdef SHT_AXISYM
/// Only the \b axisymmetric (m=0) component is transformed, and the resulting spatial fields have size NLAT.
  #endif

/// Truncation and spatial discretization are defined by \ref shtns_create and \ref shtns_set_grid_*
/// \param[in] shtns = a configuration created by \ref shtns_create with a grid set by shtns_set_grid_*
/// \param[in] Slm = spherical harmonics coefficients of \b Spheroidal scalar :
/// complex double array of size shtns->nlm [unmodified].
  #ifndef SHT_AXISYM
/// \param[out] Vt, Vp = (theta,phi)-components of spatial vector : double arrays.
  #else
/// \param[out] Vt = theta-component of spatial vector : double array.
  #endif
  #ifdef SHT_VAR_LTR
/// \param[in] llim = specify maximum degree of spherical harmonic. llim must be at most LMAX, and all spherical harmonic degree higher than llim are ignored. 
  #else
/// \param[in] llim MUST be shtns->lmax.
  #endif

// MTR_DCT : <0 => SHT_NO_DCT must be defined !!! mem only transform
//            0 => dct for m=0 only
//            m => dct up to m, (!!! MTR_DCT <= MTR !!!)

  #ifndef SHT_GRAD
	static void GEN3(SHsphtor_to_spat_,ID_NME,SUFFIX)(shtns_cfg shtns, cplx *Slm, cplx *Tlm, double *Vt, double *Vp, long int llim) {
  #else
	static void GEN3(SHsph_to_spat_,ID_NME,SUFFIX)(shtns_cfg shtns, cplx *Slm, double *Vt, double *Vp, long int llim) {
  #endif

  #ifndef SHT_AXISYM
// with m>0, 3 components not possible with DCT acceleration
	v2d *BtF, *BpF;
	struct DtDp *dyl;
	cplx te,to, pe,po;
	cplx dte,dto, dpe,dpo;
	#define BT0(i) ((double *)BtF)[2*(i)]
	#define BP0(i) ((double *)BpF)[2*(i)]
	unsigned im, imlim;
	int imlim_dct;
  #else
	v2d *BtF;
	double te,to;
	#define BT0(i) ((double *)BtF)[i]
  #endif
	long int k,m,l;
  #ifdef _GCC_VEC_
	s2d Sl0[(llim+3)>>1];		// we need some zero-padding (icc 14 bug: needs 1 more for SHT_AXISYM only!).
  #endif

  #ifndef SHT_AXISYM
	BtF = (v2d *) Vt;	BpF = (v2d *) Vp;
	if (shtns->ncplx_fft > 0) {		// alloc memory for the FFT
		BtF = VMALLOC( 2* shtns->ncplx_fft * sizeof(cplx) );
		BpF = BtF + shtns->ncplx_fft;
	}
	imlim = MTR;
	#ifdef SHT_VAR_LTR
		if (imlim*MRES > (unsigned) llim) imlim = ((unsigned) llim)/MRES;		// 32bit mul and div should be faster
	#endif
	#ifndef SHT_NO_DCT
		imlim_dct = MTR_DCT;
		#ifdef SHT_VAR_LTR
			if (imlim_dct > imlim) imlim_dct = imlim;
		#endif
	#endif
  #else
	#ifdef SHT_GRAD
		if (Vp != NULL) { k=0; do { ((s2d*)Vp)[k]=vdup(0.0); } while(++k<NLAT/2); Vp[NLAT-1]=0.0; }
	#endif
	BtF = (v2d*) Vt;
  #endif

	//	im=0;
  #ifdef _GCC_VEC_
	{	// store the m=0 coefficients in an efficient & vectorizable way.
		double* Sl = (double*) &Sl0;
		l=1;
		do {		// for m=0, compress the complex Q,S,T to double
			Sl[l-1] = (double) Slm[l];
		} while(++l<=llim);		// l=llim+1
		Sl[l-1] = 0.0;
	}
  #endif
  #ifndef SHT_NO_DCT
	{	// m=0 dct
		s2d* dyl0 = (s2d*) shtns->dykm_dct[0];		// only theta derivative (d/dphi = 0 for m=0)
	#ifndef _GCC_VEC_
		double* Sl = (double*) Slm;
		k=0;	l = 2;
		do {
			te = 0.0;	to = 0.0;
			te =  dyl0[0] * Sl[2*l-2];		// l-1
			++dyl0;
			while(l<llim) {
				to += dyl0[0] * Sl[2*l];
				te += dyl0[1] * Sl[2*l+2];
				l+=2;
				dyl0+=2;
			}
			if (l==llim) {
				to += dyl0[0] * Sl[2*l];
			}
			++dyl0;
		#ifndef SHT_AXISYM
			BT0(k) = 0.0;	BT0(k+1) = 0.0;			// required for tor or sph only transform
			BP0(k) = 0.0;	BP0(k+1) = 0.0;
		#endif
			BT0(k) = te;	BT0(k+1) = to;
		#ifdef SHT_VAR_LTR
			dyl0 += (((LMAX+1)>>1) - ((llim+1)>>1))*2;
		#endif
			k+=2;
			l = k;
		} while (k<=llim+1);
	#else
		l=0;	k=0;
		do {
			l >>= 1;	// l = l/2;
			s2d t = vdup(0.0);
			do {
				t += dyl0[l] * Sl0[l];		// { te, to }
				++l;
			} while(2*l < llim);
			dyl0 += (LMAX+1 -k)>>1;
		#ifndef SHT_AXISYM
				BT0(k) = 0.0;				BT0(k+1) = 0.0;
				BT0(k) = vlo_to_dbl(t);		BT0(k+1) = vhi_to_dbl(t);
				BP0(k) = 0.0;				BP0(k+1) = 0.0;
		#else
				*((s2d*)(((double*)BtF)+k)) = t;
		#endif
			l = k+1;
			k+=2;
		} while (l<=llim);	//	(k<=llim+1);
	#endif
		while (k<NLAT) {	// dct padding (NLAT is even)
		#ifndef SHT_AXISYM
			BT0(k) = 0.0;	BT0(k+1) = 0.0;			// required for tor or sph only transform
			BP0(k) = 0.0;	BP0(k+1) = 0.0;
		#else
			BT0(k) = 0.0;	BT0(k+1) = 0.0;
		#endif
			k+=2;
		}
    #ifdef SHT_AXISYM
		fftw_execute_r2r(shtns->idct,Vt, Vt);		// iDCT m=0
		double* st_1 = shtns->st_1;
		k=0;	do {
		#ifdef _GCC_VEC_
			v2d sin_1 = ((v2d *)st_1)[k];
			((v2d *)Vt)[k] *= sin_1;
		#else
			double sin_1 = st_1[2*k]; 	double sin_2 = st_1[2*k+1];
			Vt[2*k] *= sin_1;		Vt[2*k+1] *= sin_2;
		#endif
		} while (++k<NLAT_2);
    #endif
	}
  #else		// ifndef SHT_NO_DCT
	{	// m=0 no dct
	#ifndef _GCC_VEC_
		double* Sl = (double*) Slm;
	#endif
		k=0;
		s2d* dyl0 = (s2d*) shtns->dylm[0];	// only theta derivative (d/dphi = 0 for m=0)
		do {	// ops : NLAT_2 * [ (lmax-m+1) + 4]	: almost twice as fast.
	#ifndef _GCC_VEC_
			l=1;
			te = 0.0;	to = 0.0;
			while (l<llim) {	// compute even and odd parts
				te += dyl0[0] * Sl[2*l];
				to += dyl0[1] * Sl[2*l+2];
				l+=2;
				dyl0+=2;
			}
			if (l==llim) {
				te += dyl0[0] * Sl[2*l];
				dyl0+=2;
			}
		#ifdef SHT_VAR_LTR
			dyl0 += (((LMAX+1)>>1) - ((llim+1)>>1))*2;
		#endif
		#ifndef SHT_AXISYM
			BT0(k) = 0.0;		BP0(k) = 0.0;		// required for partial tor or sph transform
		#endif
			BT0(k) = te + to;			// Bt = dS/dt
		#ifndef SHT_AXISYM
			BT0(NLAT-1-k) = 0.0;		BP0(NLAT-1-k) = 0.0;		// required for partial tor or sph transform
		#endif
			BT0(NLAT-1-k) = te - to;
	#else
			l=0;
			s2d t = vdup(0.0);
			do {
				t += dyl0[l] * Sl0[l];		// { te, to }
				++l;
			} while (2*l < llim);
			dyl0 += (LMAX+1)>>1;
		#if __SSE3__
	/*	alternate code, which may be faster (slightly) on SSE3.	*/
			t = addi(t,t);		// { te-to , te+to }
		#ifndef SHT_AXISYM
			BT0(k) = 0.0;		BP0(k) = 0.0;		// required for partial tor or sph transform
		#endif
			BT0(k) = vhi_to_dbl(t);	// Bt = dS/dt
		#ifndef SHT_AXISYM
			BT0(NLAT-1-k) = 0.0;		BP0(NLAT-1-k) = 0.0;		// required for partial tor or sph transform
		#endif
			BT0(NLAT-1-k) = vlo_to_dbl(t);
		#else
		#ifndef SHT_AXISYM
			BT0(k) = 0.0;		BP0(k) = 0.0;		// required for partial tor or sph transform
		#endif
			BT0(k) = vhi_to_dbl(t) + vlo_to_dbl(t);	// Bt = dS/dt
		#ifndef SHT_AXISYM
			BT0(NLAT-1-k) = 0.0;		BP0(NLAT-1-k) = 0.0;		// required for partial tor or sph transform
		#endif
			BT0(NLAT-1-k) = vlo_to_dbl(t) - vhi_to_dbl(t);
		#endif
	#endif
		} while (++k < NLAT_2);
	}
  #endif		// ifndef SHT_NO_DCT

  #ifndef SHT_AXISYM
	im=1;
	BtF += NLAT;	BpF += NLAT;
    #ifndef SHT_NO_DCT
	while(im<=imlim_dct) {		// dct for im <= MTR_DCT
		m=im*MRES;
		l = LiM(shtns, 0,im);
		v2d* Sl = (v2d*) &Slm[l];
		dyl = shtns->dykm_dct[im];
		k=0;	l=m;
		do {
			v2d te = vdup(0.0);		v2d to = vdup(0.0);		v2d pe = vdup(0.0);		v2d po = vdup(0.0);
			v2d dte = vdup(0.0);	v2d dto = vdup(0.0);	v2d dpe = vdup(0.0);	v2d dpo = vdup(0.0);
			while(l<llim) {
				dpe += vdup(dyl[0].p) * Sl[l];
				to  += vdup(dyl[0].t) * Sl[l];
				dpo += vdup(dyl[1].p) * Sl[l+1];
				te  += vdup(dyl[1].t) * Sl[l+1];
				l+=2;
				dyl+=2;
			}
			if (l==llim) {
				dpe += vdup(dyl[0].p) * Sl[l];
				to  += vdup(dyl[0].t) * Sl[l];
				++dyl;
			}
			BtF[k] = addi(te, dte);		BtF[k+1] = addi(to, dto);
			BpF[k] = addi(pe, dpe);		BpF[k+1] = addi(po, dpo);
			l = (k < m) ? m : k+(m&1);
			k+=2;
		#ifdef SHT_VAR_LTR
			dyl+= (LMAX-llim);
		#endif
		} while (k<=llim+1-(m&1));
		while (k<NLAT) {		// NLAT even
			BtF[k] = vdup(0.0);		BpF[k] = vdup(0.0);
			BtF[k+1] = vdup(0.0);	BpF[k+1] = vdup(0.0);
			k+=2;
		}
		++im;
		BtF += NLAT;	BpF += NLAT;
	}
    #endif

	while(im<=imlim) {	// regular for MTR_DCT < im <= MTR
		m = im*MRES;
		l = LiM(shtns, 0,im);
		v2d* Sl = (v2d*) &Slm[l];	// virtual pointer for l=0 and im
		k=0;	l=shtns->tm[im];
		while (k<l) {	// polar optimization
			BtF[k] = vdup(0.0);		BpF[k] = vdup(0.0);
			BtF[NLAT-l + k] = vdup(0.0);		BpF[NLAT-l + k] = vdup(0.0);	// south pole zeroes
			++k;
		}
		dyl = shtns->dylm[im];
		do {	// ops : NLAT_2 * [ (lmax-m+1)*2 + 4]	: almost twice as fast.
			l=m;
			v2d dte = vdup(0.0); 	v2d dto = vdup(0.0); 	v2d pe = vdup(0.0); 	v2d po = vdup(0.0);
			v2d dpe = vdup(0.0); 	v2d dpo = vdup(0.0); 	v2d te = vdup(0.0); 	v2d to = vdup(0.0);
			while (l<llim) {	// compute even and odd parts
				dto += vdup(dyl[0].t) * Sl[l];
				pe  += vdup(dyl[0].p) * Sl[l];
				dte += vdup(dyl[1].t) * Sl[l+1];
				po  += vdup(dyl[1].p) * Sl[l+1];
				l+=2;
				dyl+=2;
			}
			if (l==llim) {
				dto += vdup(dyl[0].t) * Sl[l];
				pe  += vdup(dyl[0].p) * Sl[l];
				++dyl;
			}
			BtF[k] = addi(dte+dto, te+to);		// Bt = dS/dt       + I.m/sint *T
			BtF[NLAT-1-k] = addi(dte-dto, te-to);
			BpF[k] = addi(dpe+dpo, pe+po);		// Bp = I.m/sint * S - dT/dt
			BpF[NLAT-1-k] = addi(dpe-dpo, pe-po);
		#ifdef SHT_VAR_LTR
			dyl += (LMAX-llim);
		#endif
		} while (++k < NLAT_2);
		++im;
		BtF += NLAT;	BpF += NLAT;
	}
	for (k=0; k < NLAT*((NPHI>>1) -imlim); ++k) {	// padding for high m's
			BtF[k] = vdup(0.0);	BpF[k] = vdup(0.0);
	}
	BtF -= NLAT*(imlim+1);	BpF -= NLAT*(imlim+1);	// restore original pointer

    // NPHI > 1 as SHT_AXISYM is not defined.
    #ifndef SHT_NO_DCT
			fftw_execute_r2r(shtns->idct,(double *) BtF, (double *) BtF);		// iDCT
			fftw_execute_r2r(shtns->idct,(double *) BpF, (double *) BpF);		// iDCT
			double* st_1 = shtns->st_1;
			k=0;	do {		// m=0
				double sin_1 = st_1[k];		double sin_2 = st_1[k+1];
				((double *)BtF)[2*k] *= sin_1;   	((double *)BpF)[2*k] *= sin_1;
				((double *)BtF)[2*k+2] *= sin_2; 	((double *)BpF)[2*k+2] *= sin_2;
				k+=2;
			} while(k<NLAT);
			l = (MRES & 1) + 1;		// im-stride (l=1 if MRES even, l=2 if MRES odd)
			for (im=l; im<=MTR_DCT; im+=l) {	//even m's must be divided by sin(theta)
				k=0;	do {
					s2d sin_1 = vdup(st_1[k]);		s2d sin_2 = vdup(st_1[k+1]);
					((v2d *)BtF)[im*NLAT + k] *= sin_1;		((v2d *)BpF)[im*NLAT + k] *= sin_1;
					((v2d *)BtF)[im*NLAT + k+1] *= sin_2;	((v2d *)BpF)[im*NLAT + k+1] *= sin_2;
					k+=2;
				} while(k<NLAT);
			}
    #endif
	if (shtns->ncplx_fft >= 0) {
		fftw_execute_dft_c2r(shtns->ifft, (cplx *) BtF, Vt);
		fftw_execute_dft_c2r(shtns->ifft, (cplx *) BpF, Vp);
		if (shtns->ncplx_fft > 0) {		// free memory
			VFREE(BtF);	// this frees also BpF.
		}
	}
  #endif

	#undef BT0
	#undef BP0
  }