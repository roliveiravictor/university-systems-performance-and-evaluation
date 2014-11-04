/**********************************************************************/
/*                                                                    */
/*  Original file csqm.c:  Central Server Queueing Network Model      */
/*                                                                    */
/*  This is the queueing network simulation program of Figure 2.2     */
/*  (with a call to "reportf()" added).                               */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*                                                                    */
/*         Lecture: Systems Performance and Evaluation			      */
/*         User: Victor Rocha                                         */
/*																	  */
/*	This is program is program to simalute the behavior of			  */
/*	12 tracks based on csqm open method  							  */
/*                                                                    */
/**********************************************************************/

#include "smpl.h"
#include <stdlib.h>

//#define n0 1         /* no. class 0 tasks  */
//#define n1 0         /* no. class 1 tasks  */
#define nt 100000     /* total no. of tasks */
#define nd 12         /* no. of disk units  */
#define qd 1         /* queued req. return */

struct token
  {
    int cls;         /* task's class (& priority) */
    int un;          /* disk for current IO req.  */
    real ts;         /* tour start time stamp     */
  } task[nt+1];

int
  faixa[nd+1],        /* disk facility descriptors */
  entrada,               /* cpu facility descriptor   */
  nts=10;         /* no. of tours to simulate  */

real
  tc[12]={2.5,2.5,3.75,2.5,2.5,3.75,3.75,3.75,5.0,3.75,3.75,5.0},  /* class 0,1 mean cpu times  */
  td=30.0, sd=7.5;   /* disk time mean, std. dev. */


int main(int argc, char *argv[])
  {
   
    int i,j,event,n[3]; real t,s[3]; struct token *p;
    n[0]=n[1]=n[2]=0; s[0]=s[1]=s[2]=0.0; 
	//for (i=1; i<=nt; i++) task[i].cls=(i>n0)? 1:0;
    smpl(0,"Open Model - Oswaldo Cruz Street, Red River, Salvador-BA-Brazil");
    entrada=facility("Street Entrance",1);
        trace(2);
        stream(15);


    for (i=1; i<=nd; i++) faixa[i]=facility("Track",i);
//    for (i=1; i<=nt; i++) schedule(1,0.0,i);
    schedule(1,0.0,0);
    while (nts)
      {
        cause(&event,&i); p=&task[i];
        switch(event)
          {
			//User arrival
            case 1:  
            //p->cls=(ranf() <= 0.75)? 0:1;
              if(ranf() <= 0.5)
              p->cls=0;
              if(ranf() <= 0.85 && ranf() > 0.5)
              p->cls=1;
              if(ranf() > 0.85)
              p->cls=2;
              p->ts=stime(); 
              schedule(2,0.0,i);
              schedule(1,expntl(1.25),++i);
              break;

			//Track request
            case 2:
              j=p->cls;
              if (request(entrada,i,j)!=qd)
              schedule(3,expntl(tc[j]),i);
              break;

			//Current track release and another track selection
            case 3:
              
              release(entrada,i);
              p->un=irandom(1,nd);
              schedule(4,0.0,i);
              break;

			//Track request
            case 4:
              if (request(faixa[p->un],i,0)!=qd) then
                schedule(5,erlang(td,sd),i);
              break;

			//Track release, end tour
            case 5:  /* release Faixa, end tour */
              release(faixa[p->un],i); j=p->cls;
              t=stime(); s[j]+=t-p->ts; p->ts=t; n[j]++;
              //schedule(1,0.0,i); 
              nts--;
              break;
          }
      }
    reportf(); printf("\n\n");
    printf("Cycle time for motorcycle class = %.2f\n",s[0]/n[0]);
    printf("Cycle time for medium-sized vehicles class = %.2f\n",s[1]/n[1]);
     printf("Cycle time for large-sized vehicles class = %.2f\n",s[2]/n[2]);
    system("PAUSE");	
    return 0;
  }


