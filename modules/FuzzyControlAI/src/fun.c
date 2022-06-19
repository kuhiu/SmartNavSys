#include "fun.h"

int get_system_inputs(FILE* fdd_State, long long int *rightSensor, long long int *centerSensor, long long int *leftSensor, long long int *dir_imgproc)
{
    ssize_t lread;
    char readed[20];
    char * line = NULL;
    size_t len = 0;  

    fseek(fdd_State, 0, SEEK_SET);

    // Leo el archivo buscando las entradas
    while ( (lread=getline(&line, &len, fdd_State )) != -1)
    {
        switch (sscanf(line, "Sensores, rightSensor = %s\n", readed ))
        {
        case EOF:       // Error
            perror("sscanf");
            exit(1);
            break;
        case 0:         // No encontro
            //printf("No se encontro la linea: Sensores, rightSensor \n");
            break;
        default:        // Encontro
            *rightSensor = atof(readed);
            break;
        }

        switch (sscanf(line, "Sensores, centerSensor = %s\n", readed ))
        {
        case EOF:       // Error
            perror("sscanf");
            exit(1);
            break;
        case 0:         // No encontro
            //printf("No se encontro la linea: Sensores, centerSensor \n");
            break;
        default:        // Encontro
            *centerSensor = atof(readed);
            break;
        }

        switch (sscanf(line, "Sensores, leftSensor = %s\n", readed ))
        {
        case EOF:       // Error
            perror("sscanf");
            exit(1);
            break;
        case 0:         // No encontro
            //printf("No se encontro la linea: Sensores, leftSensor \n");
            break;
        default:        // Encontro
            *leftSensor = atof(readed);
            break;
        }

        switch (sscanf(line, "ImgProc, Direccion = %s\n", readed ))
        {
        case EOF:       // Error
            perror("sscanf");
            exit(1);
            break;
        case 0:         // No encontro
            //printf("No se encontro la linea: Sensores, rightSensor \n");
            break;
        default:        // Encontro
            *dir_imgproc = atof(readed);
            break;
        }
    }

    free(line);
    return 0;
}

int put_system_outputs(FILE* fdd_State, float dirr, float speed)
{
    ssize_t lread;
    char readed[10];
    char * line = NULL;
    size_t len = 0;  
    ssize_t loffset=0;

    size_t escrito;

    // Reunicio puntero al archivo
    fseek(fdd_State, 0, SEEK_SET);

    // Leo el archivo buscando las entradas
    while ( (lread=getline(&line, &len, fdd_State )) != -1)
    {
        loffset = loffset + lread;
        switch (sscanf(line, "Angulo requerido = %s g", readed ))
        {
        case EOF:       // Error
            perror("sscanf");
            exit(1);
            break;
        case 0:         // No encontro
            //printf("No se encontro la linea \n");
            break;
        default:        // Encontro
        //printf("Encontre \n");
          //printf("Line %s strlen %d\n", line, (int)strlen(line));
            sprintf(line, "Angulo requerido = %f", dirr); 
            fseek(fdd_State, (loffset-lread), SEEK_SET);
            //printf("Line %s strlen %d\n", line, (int)strlen(line));
            if ( ( escrito=fwrite(line, sizeof(char), strlen(line), fdd_State)) != strlen(line))
            {
                printf("Error escribiendo %d\n", (int)escrito);
                return -1;
            }
            fseek(fdd_State, (loffset), SEEK_SET);
            break;
        }

        switch (sscanf(line, "Pwm, Velocidad = %s", readed ))
        {
        case EOF:       // Error
            perror("sscanf");
            exit(1);
            break;
        case 0:         // No encontro
            //printf("No se encontro la linea \n");
            break;
        default:        // Encontro
        //printf("Encontre \n");
          //printf("Line %s strlen %d\n", line, (int)strlen(line));
            sprintf(line, "Pwm, Velocidad = %f", (dirr)*100); 
            fseek(fdd_State, (loffset-lread), SEEK_SET);
            //printf("Line %s strlen %d\n", line, (int)strlen(line));
            if ( ( escrito=fwrite(line, sizeof(char), strlen(line), fdd_State)) != strlen(line))
            {
                printf("Error escribiendo %d\n", (int)escrito);
                return -1;
            }
            fseek(fdd_State, (loffset), SEEK_SET);
            break;
        }
    }
    free(line);
    return 0;
}

struct io_type* initialize_system_io(char *name, float value, struct mf_type *membership_functions, struct io_type *next)
{
    struct io_type *System_Inputs = malloc (sizeof (struct io_type));  // System inputs

    if (System_Inputs == NULL)
        return NULL;
    strcpy(System_Inputs->name, name);
    System_Inputs->value = value;
    System_Inputs->membership_functions = membership_functions;
    System_Inputs->next =  next;
    return System_Inputs;
}

void initialize_membership_inputs(char *name, int value, float point1, float point2, float slope1, float slope2, struct mf_type * next, struct mf_type** membership_functions_inputs)
{
    *membership_functions_inputs = malloc (sizeof (struct mf_type)); 

    strcpy((*membership_functions_inputs)->name, name);
    (*membership_functions_inputs) -> value  = value;
    (*membership_functions_inputs) -> point1 = point1;
    (*membership_functions_inputs) -> point2 = point2;
    (*membership_functions_inputs) -> slope1 = slope1;
    (*membership_functions_inputs) -> slope2 = slope2;
    (*membership_functions_inputs) -> next   = next;

    return;
}

// Calcula el grado de pertenencia 
/* Compute Degree of Membership--Degree to which input is a member of mf is
calculated as follows: 1. Compute delta terms to determine if input is inside
or outside membership function. 2. If outside, then degree of membership is 0.
Otherwise, smaller of delta_1 * slope1 and delta_2 * slope2 applies.
3. Enforce upper limit. */
void compute_degree_of_membership(struct mf_type *mf, int input)
{
    float delta_1;
    float delta_2;
    delta_1 = input - mf->point1;
    delta_2 = mf->point2 - input;
    
    //printf("delta_1 %f, delta_2 %f \n", delta_1, delta_2);
    //printf("input %d, mf->point1 %d, mf->point2 %d \n", input, mf->point1, mf->point2);

    if ((delta_1 <= 0) || (delta_2 <= 0))   // La entrada esta fuera de la funcion de pertenencia 
        mf->value = 0;                      // El grado de pertenencia es 0
    else
    {
        //printf("(mf->slope1*delta_1) %f , (mf->slope2*delta_2) %f \n", (mf->slope1*delta_1), (mf->slope2*delta_2));
        mf->value = fmin( (mf->slope1*delta_1), (mf->slope2*delta_2) );  // Me fijo si se encuentra en alguna de las 2 pendientes
        //printf("mf->value %f \n", mf->value);
        mf->value = fmin( mf->value, UPPER_LIMIT);  // Que jamas se pase del valor maximo de pertenencia       
        //printf("mf->value %f \n", mf->value);
    }

    return;
}


// Fuzzification-- El grado de pertenencia es calculado para cada funcion de pertenencia.
// Los valores corresponden a los antecedentes en las reglas.
void fuzzification(struct io_type *System_Inputs)
{
    struct io_type *si = NULL;    /* system input pointer        */
    struct mf_type *mf = NULL;    /* membership function pointer */
    for(si=System_Inputs; si != NULL; si=si->next)    // Para cada entrada
    {
        for(mf=si->membership_functions; mf != NULL; mf=mf->next)   // Para cada funcion de pertenencia de cada entrada 
        {
            compute_degree_of_membership(mf,si->value);
            //printf("Entrada: %s, valor: %f - Pertenencia: %s, grado de pertenencia %f\n", si->name, si->value, mf->name, mf->value);
        }
    }
}

// Cada regla consiste de una lista de punteros de antecedentes y consecuentes 
// Cuando una regla es evaluada, sus antecedentes son ANDed ( Si temperatura es bajo, la presion alta... etc) y se consigue la fuerza 
// entonces, la fuerza se aplica a cada regla de salida
/* Rule Evaluation--Each rule consists of a list of pointers to antecedents
(if side), list of pointers to outputs (then side), and pointer to next rule
in rule base. When a rule is evaluated, its antecedents are ANDed together,
using a minimum function, to form strength of rule. Then strength is applied
to each of listed rule outputs. If an output has already been assigned a rule
strength, during current inference pass, a maximum function is used to
determine which strength should apply. */
void rule_evaluation(struct rule_type *Rule_Base, struct io_type *System_Outputs)
{
    struct rule_type *rule;
    struct rule_element_type *ip;       /* pointer of antecedents  (if-parts)   */
    struct rule_element_type *tp;       /* pointer to consequences (then-parts) */
    float strength;                    /* strength of  rule currently being evaluated */

    for(rule=Rule_Base; rule != NULL; rule=rule->next){
        strength = UPPER_LIMIT;                       /* max rule strength allowed */
        /* process if-side of rule to determine strength */
        for(ip=rule->if_side; ip != NULL; ip=ip->next){
            strength = fmin(strength,*(ip->value));
            //DEBUG_PRINT(("strength del if %f\n", strength));
        }
        //DEBUG_PRINT(("strength de la regla %f\n", strength));
        /* process then-side of rule to apply strength */
        for(tp=rule->then_side; tp != NULL; tp=tp->next){
            *(tp->value) = fmax(strength,*(tp->value));
            //DEBUG_PRINT(("then %f\n", *(tp->value)));
        }
    }
    return;
}


// Defuzzificacion
void defuzzification(struct io_type *System_Outputs)
{
    struct io_type *so;    // Puntero a la salida del sistema 
    struct mf_type *mf;    // Puntero a las funciones de pertenencia de salida  
    
    float sum_of_products;   //   /* sum of products of area & centroid */
    float sum_of_areas;     /* sum of shortend trapezoid area          */
    float area;
    float centroid;

    //printf("Holi \n");
    // Calcula el valor de defuzzificacion por cada salida del sistema
    // Metodo del centro de gravedad, se calcula el centroide en el eje X para cada funcion de 
    // pertenencia. Luego, cada funcion de pertenencia esta limitada en altura por la fuerza de 
    // la regla aplicada, y se calcula el area.
    for(so=System_Outputs; so != NULL; so=so->next)
    {
        //printf("Llegue \n");
        sum_of_products = 0;
        sum_of_areas = 0;
        for(mf=so->membership_functions; mf != NULL; mf=mf->next)
        {
            //printf("mf value: %d\n", mf->value);
            area = compute_area_of_trapezoid(mf);
            centroid = mf->point1 + (mf->point2 - mf->point1)/2;
            sum_of_products += area * centroid;
            sum_of_areas += area;
            //printf("mf: %d %d %f\n", mf->point1, mf->point2, mf->value);
            //printf("area: %d, centroid: %d\n", area, centroid);
        }
        //printf("Llegue sum_of_products: %f, sum_of_areas: %f\n", sum_of_products, sum_of_areas);
        so->value = sum_of_products/sum_of_areas;   /* weighted average */
        //printf("Salida: %f\n", so->value);
    }

    return;
}

// /* Compute Area of Trapezoid--Each inference pass produces a new set of output
// strengths which affect the areas of trapezoidal membership functions used in
// center-of-gravity defuzzification. Area values must be recalculated with each
// pass. Area of trapezoid is h*(a+b)/2 where h=height=output_strength=mf->value
// b=base=mf->point2-mf->point1 a=top= must be derived from h,b, and slopes1&2 */
float compute_area_of_trapezoid(struct mf_type *mf)
{
    float run_1;
    float run_2;
    float base;
    float top;
    float area;
    // h*(a+b)/2
    base = mf->point2 - mf->point1; // a
    run_1 = mf->value/mf->slope1;   // Ax = Ay * pendiente
    run_2 = mf->value/mf->slope2;   // Ax = Ay * pendiente
    top = base-run_1-run_2;         // b 
    area = mf->value*(base + top)/2;
    return(area);
}

void add_rule (struct rule_type **rule, char *name, struct rule_element_type *if_side, struct rule_element_type *then_side, struct rule_type *next)
{
    *rule = malloc (sizeof (struct rule_type));    // Lista de todas las reglas
    strcpy( (*rule)->name, name); 
    (*rule) -> if_side   = if_side;
    (*rule)-> then_side = then_side;
    (*rule) -> next      = next;

    return;
}

void update_rule (struct rule_type *rule, char *name){
    strcpy( rule->name, name); 
    return;
}

void add_rule_element( struct rule_element_type **element,  float value, struct rule_element_type *next)
{
    *element = malloc (sizeof (struct rule_element_type));    // Lista de todas las reglas
    //(*element) -> value = malloc (sizeof(float) );

    //*((*element) -> value) = value;
    (*element) -> next  = next;

    return;
}

void charge_rule(   char *rule_text,                     
                    struct rule_element_type **if_side_right, struct rule_element_type **if_side_center, struct rule_element_type **if_side_left, struct rule_element_type **if_side_imgproc,
                    struct rule_element_type **then_side_dirr, struct rule_element_type **then_side_speed,
                    struct rule_type **rule, struct rule_type *next_rule ) {
    
    // Cargo cada regla con los valores de la fuzzificacion
    add_rule_element(if_side_imgproc, 0.0, NULL);
    add_rule_element(if_side_right,   0.0, *if_side_imgproc);
    add_rule_element(if_side_center,  0.0, *if_side_right);
    add_rule_element(if_side_left,    0.0, *if_side_center);
    add_rule_element(then_side_speed, 0.0, NULL);
    add_rule_element(then_side_dirr,  0.0, *then_side_speed);
    add_rule(rule, rule_text, *if_side_left, *then_side_dirr, next_rule);
    return;
}

void read_from_state_string(FILE* fdd_State, char recurso[], struct sembuf sb, int semid, char *readed){
    ssize_t lread;
    char * line = NULL;
    size_t len = 0;  

    // Tomo el recurso
    sb.sem_op = -1;         
    if (semop(semid, &sb, 1) == -1) {          
        perror("semop");
        exit(1);
    }

    // Leo el state.txt
    // Antes de empezar el ciclo, veo cual es el angulo absoluto del robot y la distancia que ya recorrio
    fseek(fdd_State, 0, SEEK_SET);
    while ( (lread=getline(&line, &len, fdd_State )) != -1)
    {
        switch (sscanf(line, recurso, readed ))
        {
        case EOF:       // Error
            perror("sscanf");
            exit(1);
            break;
        case 0:         // No encontro
            //printf("No se encontro la linea: Sensores, rightSensor \n");
            break;
        default:        // Encontro
            break;
        }
    }
    free(line);
    // Libero el recurso
    sb.sem_op = 1;          
    if (semop(semid, &sb, 1) == -1) {
        perror("semop");
        exit(1);
    }

    return;
}