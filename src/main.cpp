//std
#include <cmath>
#include <cstdio>

//data
FILE *fin, *fout;
static double *qn, *dn;
static unsigned nt, lt, m[2];
static double w, h, t, E, v, D, q, s[2] = {0, 0}, x[2];

static void header(void)
{
	printf("**************************************************\n");
	printf("                     PLATE SS                     \n");
	printf("**************************************************\n");
	printf("Author: Dr. M. Santana                            \n");
	printf("**************************************************\n");
}
static void quitter(void)
{
	printf("**************************************************\n");
	printf("Thanks for using PLATE SS - Have a nice day! :)   \n");
	printf("**************************************************\n");
}

static bool input_geometry(void)
{
	fprintf(fout, "Geometry:\n");
	fprintf(fout, "\tWidth: ");
	fscanf(fin, "%lf", &w);
	fprintf(fout, "\tHeight: ");
	fscanf(fin, "%lf", &h);
	fprintf(fout, "\tThickness: ");
	fscanf(fin, "%lf", &t);
	if(w < 0 || h < 0 || t < 0)
	{
		printf("\tInvalid input!\n");
		return false;
	}
	return true;
}
static bool input_material(void)
{
	fprintf(fout, "Material:\n");
	fprintf(fout, "\tPoisson ratio: ");
	fscanf(fin, "%lf", &v);
	fprintf(fout, "\tElastic modulus: ");
	fscanf(fin, "%lf", &E);
	if(v < 0 || v > 0.5 || E < 0)
	{
		fprintf(fout, "\tInvalid input!\n");
		return false;
	}
	D = E * t * t * t / 12 / (1 - v * v);
	return true;
}
static bool input_load(void)
{
	fprintf(fout, "Load:\n");
	fprintf(fout, "\tLoad types:\n");
	fprintf(fout, "\t(1) Point\n");
	fprintf(fout, "\t(2) Line H\n");
	fprintf(fout, "\t(3) Line V\n");
	fprintf(fout, "\t(4) Patch\n");
	fprintf(fout, "\tType: ");
	fscanf(fin, "%d", &lt);
	fprintf(fout, "\tValue:");
	fscanf(fin, "%lf", &q);
	fprintf(fout, "\tPosition: ");
	fscanf(fin, "%lf %lf", &x[0], &x[1]);
	if(lt == 2)
	{
		fprintf(fout, "\tSize: ");
		fscanf(fin, "%lf", &s[0]);
	}
	if(lt == 3)
	{
		fprintf(fout, "\tSize: ");
		fscanf(fin, "%lf", &s[1]);
	}
	if(lt == 4)
	{
		fprintf(fout, "\tSize: ");
		fscanf(fin, "%lf %lf", &s[0], &s[1]);
	}
	if(x[0] - s[0] / 2 < 0 || x[0] + s[0] / 2 > w || x[1] + s[1] / 2 < 0 || x[1] - s[1] / 2 > h)
	{
		fprintf(fout, "\tInvalid input!\n");
		return false;
	}
	return true;
}
static bool input_solution(void)
{
	fprintf(fout, "Solution:\n");
	fprintf(fout, "\tTerms: ");
	fscanf(fin, "%d", &nt);
	fprintf(fout, "\tMesh: ");
	fscanf(fin, "%d %d", &m[0], &m[1]);
	if(nt == 0 || m[0] == 0 || m[1] == 0)
	{
		fprintf(fout, "\tInvalid input!\n");
		return false;
	}
	return true;
}

static void setup(void)
{
	qn = new double[nt * nt];
	dn = new double[nt * nt];
}
static void solve(void)
{
	for(unsigned i = 0; i < nt; i++)
	{
		for(unsigned j = 0; j < nt; j++)
		{
			const double s1 = s[0];
			const double s2 = s[1];
			const double x1 = x[0];
			const double x2 = x[1];
			const unsigned a = i + 1;
			const unsigned b = j + 1;
			const double r1 = a * M_PI / w;
			const double r2 = b * M_PI / h;
			if(lt == 1)
			{
				qn[nt * i + j] = q / w / h * sin(a * M_PI * x1 / w) * sin(b * M_PI * x2 / h);
			}
			if(lt == 4)
			{
				qn[nt * i + j] = q / s1 / s2 * sin(r1 * x1) * sin(r2 * x2) * sin(r1 * s1 / 2) * sin(r2 * s2 / 2);
			}
			dn[nt * i + j] = qn[nt * i + j] / D / pow(pow(a * M_PI / w, 2) + pow(b * M_PI / h, 2), 2);
		}
	}
}
static void derived(void)
{
	FILE* file[9];
	const unsigned a = m[0] + 1;
	const unsigned b = m[1] + 1;
	file[0] = fopen("u3.txt", "w");
	file[1] = fopen("t1.txt", "w");
	file[2] = fopen("t2.txt", "w");
	file[3] = fopen("k11.txt", "w");
	file[4] = fopen("k22.txt", "w");
	file[5] = fopen("k12.txt", "w");
	file[6] = fopen("M11.txt", "w");
	file[7] = fopen("M22.txt", "w");
	file[8] = fopen("M12.txt", "w");
	for(unsigned i = 0; i <= m[1]; i++)
	{
		for(unsigned j = 0; j <= m[0]; j++)
		{
			double dp[] = {0, 0, 0};
			double ep[] = {0, 0, 0};
			double sp[] = {0, 0, 0};
			const double x1 = j * w / m[0];
			const double x2 = i * h / m[1];
			for(unsigned k = 0; k < nt; k++)
			{
				for(unsigned p = 0; p < nt; p++)
				{
					const double s1 = (k + 1) * M_PI / w;
					const double s2 = (p + 1) * M_PI / h;
					dp[0] += dn[nt * k + p] * sin(s1 * x1) * sin(s2 * x2);
					dp[1] += dn[nt * k + p] * s2 * sin(s1 * x1) * cos(s2 * x2);
					dp[2] -= dn[nt * k + p] * s1 * cos(s1 * x1) * sin(s2 * x2);
					ep[0] += dn[nt * k + p] * s1 * s1 * sin(s1 * x1) * sin(s2 * x2);
					ep[1] += dn[nt * k + p] * s2 * s2 * sin(s1 * x1) * sin(s2 * x2);
					ep[2] += dn[nt * k + p] * 2 * s1 * s1 * sin(s1 * x1) * sin(s2 * x2);
				}
			}
			sp[0] = D * (ep[0] + v * ep[1]);
			sp[1] = D * (ep[1] + v * ep[0]);
			sp[2] = D * (1 - v * v) / 2 / (1 + 2 * v) * ep[2];
			for(unsigned k = 0; k < 3; k++)
			{
				fprintf(file[k + 0], "%+.6e %+.6e %+.6e\n", x1, x2, dp[k]);
				fprintf(file[k + 3], "%+.6e %+.6e %+.6e\n", x1, x2, ep[k]);
				fprintf(file[k + 6], "%+.6e %+.6e %+.6e\n", x1, x2, sp[k]);
			}
		}
		for(unsigned i = 0; i < 9; i++)
		{
			fprintf(file[i], "\n");
		}
	}
	for(unsigned i = 0; i < 9; i++)
	{
		fclose(file[i]);
	}
}
static void cleanup(void)
{
	delete[] qn;
	delete[] dn;
}

int main(int argc, char** argv)
{
	//input
	header();
	fin = argc == 1 ? stdin : fopen(argv[1], "r");
	fout = argc == 1 ? stdout : fopen(argv[2], "w");
	bool test = input_geometry() && input_material() && input_load() && input_solution();
	if(!test) return 0;
	//solve
	setup();
	solve();
	derived();
	cleanup();
	quitter();
	//plot
	system("gnuplot -p -e \"file='u3.txt'; name = 'u_3'\" plot.gp > nul");
	system("gnuplot -p -e \"file='t1.txt'; name = '{/Symbol q}_1'\" plot.gp > nul");
	system("gnuplot -p -e \"file='t2.txt'; name = '{/Symbol q}_2'\" plot.gp > nul");
	system("gnuplot -p -e \"file='k11.txt'; name = '{/Symbol k}_{11}'\" plot.gp > nul");
	system("gnuplot -p -e \"file='k22.txt'; name = '{/Symbol k}_{22}'\" plot.gp > nul");
	system("gnuplot -p -e \"file='k12.txt'; name = '{/Symbol k}_{12}'\" plot.gp > nul");
	system("gnuplot -p -e \"file='M11.txt'; name = 'M_{11}'\" plot.gp > nul");
	system("gnuplot -p -e \"file='M12.txt'; name = 'M_{22}'\" plot.gp > nul");
	system("gnuplot -p -e \"file='M22.txt'; name = 'M_{12}'\" plot.gp > nul");
	//return
	return 0;
}