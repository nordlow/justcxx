#ifdef HAVE_ALTIVEC

/*!
 * Compile with -faltivec
 */
float sumVAltivec2(float a[], int n)
{
  register vector float vb;
  /* ensures intermediate sum vb stays in register */
  vector float dest;
  /* Declare vector in memory to move contents out of register */
  float *p = (float *)(&dest);
  /* set pointer to it in order to add the four floats             */
  vector float *input=(vector float *)a;
  /* Set up input pointer at array address */

  vb=(vector float)vec_splat_u32(0);
  /* clear intermediate sum (integer 0 == float 0)*/
  for (int i=0; i < n; i+=4) {
    vb = vec_add(*input++,vb);
    /* c = vec_add(a, b);*/
  }
  dest=vb;
  return *p+*(p+1)+*(p+2)+*(p+3);
  /* compacting phase */
}

#endif
