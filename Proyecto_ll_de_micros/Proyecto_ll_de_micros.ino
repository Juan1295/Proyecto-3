int tri1=13;
int echo1=12;
int tri2=11;
int echo2=10;
int din1=9;  //Naranja
int cs1=8;   //Rojo
int clk1=7;  //Gris
int din2=6;  //Negro
int cs2=5;   //Morado
int clk2=4;  //Cafe
int f=1,c=4,a=1,b=1;
int dis1,dis2;
int rloj=3; //Azul
int data=2; //Amarillo
int gol1=0,gol2=0;
int dif1=0,dif2=0,acumdif=0;
int posant1=0,posant2=0;
int velbola=0;

void setup()
{
  Serial.begin(9600);
  pinMode(din1, OUTPUT);
  pinMode(cs1, OUTPUT); 
  pinMode(clk1, OUTPUT); 
  pinMode(din2, OUTPUT);
  pinMode(cs2, OUTPUT); 
  pinMode(clk2, OUTPUT); 
  pinMode(tri1, OUTPUT);
  pinMode(echo1, INPUT); 
  pinMode(tri2, OUTPUT);
  pinMode(echo2, INPUT); 
  pinMode(rloj, OUTPUT);
  pinMode(data, OUTPUT);
  writeData1(0x0B,7);  //Scan1 0 ... 7
  writeData1(0x0C,1);  //Normal operation1
  writeData2(0x0B,7);  //Scan2 0 ... 7
  writeData2(0x0C,1);  //Normal operation2
  ClearJR();
}

void loop()
{ 
  delay(25);
  dis1=Sensor(tri1,echo1);
  dis2=Sensor(tri2,echo2);
  Clear1();
  Clear2();
  Barra1(dis1);
  Barra2(dis2);
  dif1=(posant1-dis1)*4;
  if(dif1<0)
    dif1=dif1*-1;
  
  dif2=(posant2-dis2)*4;
  
  if(dif2<0)
    dif2=dif2*-1;

  posant1=dis1;
  posant2=dis2;
  velbola+=1;
  if(velbola>=14)
  {
    Bola();
    if(f==7)
    {
        acumdif+=dif1;
    }
    if(f==-6)
    {
        acumdif+=dif2;
    }
    velbola=0+acumdif;
    Gol();
  } 
  if((f<9) && (f>0))
    writeData1(f,1<<c);

  else
    writeData2(f+8,1<<c);

}

void writeData1(int address, int value)//Maanejador de matriz 1
{
    uint16_t data =(address<<8) | (value); 
    
    digitalWrite(cs1, LOW);

    for(int i=0; i<16; i++)
    {
      if( data&(1<<(15-i)) )
        digitalWrite(din1, HIGH);
      else
        digitalWrite(din1, LOW);

      digitalWrite(clk1, HIGH);
      digitalWrite(clk1, LOW);
    }
    
    digitalWrite(cs1, HIGH);
}

void writeData2(int address, int value)//Maanejador de matriz 2
{
    uint16_t data =(address<<8) | (value); 
    
    digitalWrite(cs2, LOW);

    for(int i=0; i<16; i++)
    {
      if( data&(1<<(15-i)) )
        digitalWrite(din2, HIGH);
      else
        digitalWrite(din2, LOW);

      digitalWrite(clk2, HIGH);
      digitalWrite(clk2, LOW);
    }
    
    digitalWrite(cs2, HIGH);
}

void Clear1()
{
  for(int i=1;i<9;i++)
  {
    writeData1(i,0);
  }
}

void Clear2()
{
  for(int i=1;i<9;i++)
  {
    writeData2(i,0);
  }
}

void Barra1(int dis)
{
  writeData1(8,3<<6-dis );
}

void Barra2(int dis)
{
  writeData2(1,3<<6-dis);
}

void Bola()
{
 if(f==7)
  {
    a=a*-1;
  }
  if(f==-6)
  {
    a=a*-1;
  }
  if(c==0)
  {
    b=b*-1;
  }
  if(c==7)
  {
    b=b*-1;
  }
  f=f+a;
  c=c+b;
}

int Sensor(int tri,int echo)
{
  long distancia;
  long tiempo;
  digitalWrite(tri,LOW); /* Por cuestión de estabilización del sensor*/
  delayMicroseconds(5);
  digitalWrite(tri, HIGH); /* envío del pulso ultrasónico*/
  delayMicroseconds(10);
  tiempo=pulseIn(echo, HIGH); /* Función para medir la longitud del pulso entrante. Mide el tiempo que transcurrido entre el envío
  del pulso ultrasónico y cuando el sensor recibe el rebote, es decir: desde que el pin empieza a recibir el rebote, HIGH, hasta que
  deja de hacerlo, LOW, la longitud del pulso entrante*/
  distancia= int(0.017*tiempo); /*fórmula para calcular la distancia obteniendo un valor entero*/
  /*Monitorización en centímetros por el monitor serial*/
  if(distancia<2)
    distancia=2;

  if(distancia>26)
   distancia=26;

  distancia=(distancia-2)/4; 
  return distancia;
}

void RegistroJR(uint8_t info)
{
  for(int i=0;i<8;i++)
  {
      if( info&(1<<(i)) )
        digitalWrite(data, HIGH);
      else
        digitalWrite(data, LOW);

      digitalWrite(rloj, HIGH);
      digitalWrite(rloj, LOW);
  }
  digitalWrite(data, LOW);
}

void ClearJR()
{
  int a=0x00;
  RegistroJR(a);
}

void Gol()
{
  if((f==-6) && (c!=6-dis2) && (c!=7-dis2))// Gol para jugador 1.
  {
    if((f<9) && (f>0))
    {
      writeData1(f,1<<c);
    }
    else
    {
      writeData2(f+8,1<<c);
    }
    if(((b==-1) && (c==(8-dis2))) || ((b==1) && (c==(5-dis2))))
    {
      b=b*-1;
    }
    else
    {
      gol1+=1;
      f=1;
      c=4;
      a=a*-1;
      velbola=acumdif=0;    
      if(gol1==10)
      {
        gol1=gol2=0;
        delay(2000);
      }
      RegistroJR(gol2<<4|gol1);
      delay(2000);
    }
  }
  if((f==7) && ((c!=6-dis1) && ((c!=7-dis1))))
  {
   if((f<9) && (f>0))
    {
      writeData1(f,1<<c);
    }
    else
    {
      writeData2(f+8,1<<c);
    }
    if(((b==-1) && (c==(8-dis1))) || ((b==1) && (c==(5-dis1))))
    {
      b=b*-1;
    }
    else
    {
      gol2+=1;
      f=0;
      c=3;
      a=a*-1;
      velbola=acumdif=0;
      if(gol2==10)
      {
        gol1=gol2=0;
        delay(2000);
      }
    RegistroJR(gol2<<4|gol1);
    delay(2000);
    }
  }
}


