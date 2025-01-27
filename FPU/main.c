#include<stdio.h>
#include<math.h>
#include<locale.h>
#include<conio.h>

/// <summary>
/// Fun��o para imprimir na tela os bits de um n�mero
/// </summary>
/// <param name="n">valor decimal qualquer</param>
/// <param name="i">numero de bits que ser� implementado o n�mero</param>
/// <returns></returns>
void binario(int n, int i)
{
	int k;
	for (i--; i >= 0; i--)
	{
		k = n >> i; //valor de n deslocado em i
		if (k & 1) //1 � uma m�scara, n�mero 1 em bin�rio 00000001
			printf("1");
		else
			printf("0");
			
	}
}

void binario_s(int n, int i, char *s) {
	printf("\n%s\t", s);
	binario(n, i);
}

typedef union {
	float f; //f e a struct field ocupam a mesma posi��o da mem�ria
	struct {
		unsigned int mantissa : 23;
		unsigned int exponente : 8;
		unsigned int sinal : 1;
	} field;
} unionfloat;

/// <summary>
/// Converte o n�mero no padr�o iee754 para o n�mero com ponto flutuante normal
/// </summary>
/// <param name="numero">n�mero no padr�o iee754</param>
/// <returns></returns>
float reconstruirNumero(unionfloat numero) {
	return pow(-1, (numero.field.sinal)) * (1.0 + numero.field.mantissa / pow(2, 23)) * pow(2, (numero.field.exponente - 127));
}

/// <summary>
/// Normalizar o n�mero a partir do sinal, expoente e mantissa, colocando o primeiro bit como 1
/// </summary>
/// <returns></returns>
unionfloat normalizaNumero(int sinal, int exp, int mantissa) {
	unionfloat n;
	n.field.sinal = sinal;
	int desloc_exp = 0;
	
	// Se n�o for zero (n�o faz sentido procurar)
	if (mantissa != 0) {
		// Procura pelo primeiro 1 da mantissa, da esquerda (MSB) para direita (LSB)
		int mascara = 0x80000000;
		for (int i = 31; i >= 0; i--) {
			if ((mantissa & mascara) != 0) {
				desloc_exp = i;
				break;
			}
			mascara = mascara >> 1;
		}
		// Calcula deslocamento em rela��o � posi��o padr�o (23)
		desloc_exp = desloc_exp - 23;
		if (desloc_exp < 0)
			mantissa = mantissa << abs(desloc_exp);
		else 
			mantissa = mantissa >> desloc_exp;
		n.field.exponente = exp + desloc_exp + 127;
	} else {
		n.field.exponente = 0;
	}

	n.field.mantissa = mantissa;

	return n;
}

/// <summary>
/// Realiza o complemento
/// </summary>
/// <returns></returns>
void ajustaNegativo(int * mantissa, int * exp) {
	*mantissa = ~(*mantissa) + 1;
}

/// <summary>
/// Opera��o de soma de dois n�meros com ponto flutuante
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
unionfloat add(unionfloat a, unionfloat b) {

	int exp;
	int exp2;
	int desloca;
	int mantissa_aux;
	int mantissa_aux2;
	int soma_mantissa;

	exp = a.field.exponente - 127;// recupera o expoente limpo  (valora armazenado na variavel expoente - bias(127))
	exp2 = b.field.exponente - 127;

	mantissa_aux = a.field.mantissa + 0x800000;//passando para variavel auxiliar e somando 1.  (1.mantissa implicito que deve ser adicionado
	mantissa_aux2 = b.field.mantissa + 0x800000;//passando para variavel auxiliar e somando 1.

	// Se for um n�mero negativo
	if (a.field.sinal == 1) {
		ajustaNegativo(&mantissa_aux, &exp);
	}
	// Se for um n�mero negativo
	if (b.field.sinal == 1) {
		ajustaNegativo(&mantissa_aux2, &exp2);
	}

	if (exp != exp2) {
		//se os expoentes forem diferentes, tem que pegar o menor e igualar ao maior
		if (exp > exp2) {  // iguala o expoente dos numeros para poder somar (so podemos somar as mantissas se tiverem expoentes iguais) nesse caso exp do numero 1 � maior que o do numero 2
			desloca = exp - exp2; //quantas vezes tem que deslocar essa mantissa
			exp2 = exp;
			mantissa_aux2 = mantissa_aux2 >> desloca; // ent�o deslocamos a v�rgula do numero menor (2)
		}
		if (exp < exp2) { // iguala o expoente dos numeros para poder somar (so podemos somar as mantissas se tiverem expoentes iguais) nesse caso exp do numero 2 � maior que o do numero 1
			desloca = exp2 - exp; //quantas vezes tem que deslocar essa mantissa
			exp = exp2;
			mantissa_aux = mantissa_aux >> desloca;  // ent�o deslocamos a v�rgula do numero menor (1)
		}
	} 

	soma_mantissa = mantissa_aux + mantissa_aux2;

	int sinal = 0;
	// S� faz o complemento se o n�mero for negativo
	if (soma_mantissa < 0) {
		soma_mantissa = ~soma_mantissa;
		sinal = 1;
	}
	
	return normalizaNumero(sinal, exp, soma_mantissa);
}

void exibeNumero(unionfloat n, char * descricao) {
	printf("\n");
	printf("%d ", n.field.sinal);
	binario(n.field.exponente, 8);
	printf(" ");
	binario(n.field.mantissa, 23);
	printf("\n");
	printf("%s reconstitu�do:  \t%f\n", descricao, reconstruirNumero(n));
	printf("\n");
}

int main() {
    setlocale(LC_ALL, "Portuguese");

	unionfloat numero1;
	unionfloat numero2;
	char op;

	unionfloat numeroResultadoOperacao;

	printf("Entre com um ponto flutuante 1 (com v�rgula): ");
	scanf("%f", &numero1.f);
	printf("Entre com um ponto flutuante 2 (com v�rgula): ");
	scanf("%f", &numero2.f);
	printf("Informe a opera��o (+/-): ");
	op = getch();
	printf("%c", op);
	
	if (op == '-') {
		numero2.f = - numero2.f;
	}
	printf("\n\n");
	
	exibeNumero(numero1, "Numero 1");
	exibeNumero(numero2, "Numero 2");

	numeroResultadoOperacao = add(numero1, numero2);
	
	exibeNumero(numeroResultadoOperacao, "Resultado (simulado)");
	return 0;
}
