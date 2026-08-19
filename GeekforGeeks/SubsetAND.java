import java.util.Scanner;

class TestClass {
   public static void main(String args[] ) throws Exception
    {
                       Scanner input = new Scanner(System.in);
                       int T=input.nextInt();
     while(T-- >0){
       int z= input.nextInt();
       int n=input.nextInt();
       int a[]=new int[n];
       int flag=0;
       for(int i=0;i<n;i++){
            a[i]=input.nextInt();
       }
       for(int i=0;i<n;i++){
           if((z=(z&a[i]))==0){
                flag=1;
                break;
           }
       }
       if(flag==1){
               System.out.println("Yes");
       }
       else{
               System.out.println("No");
       }
    }
   }
}
