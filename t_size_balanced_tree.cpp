#include "size_balanced_tree.hpp"

void test_size_balanced_tree()
{
    freopen("in.txt","r", stdin);
    freopen("out.txt","w", stdout);
    long x,n;
    char ch;
    scanf("%ld\n",&n);
    No=new NODE;
    No->size=0;
    No->key=0;
    No->left=No;
    No->right=No;
    tree = No;
    while (n--)
    {
        scanf("%c",&ch);
        if (ch!='I' && ch!='D' && ch!='F' && ch!='R' && ch!='T' && ch!='P' && ch!='S') scanf("%c",&ch);
        scanf("%ld\n",&x);
        switch(ch)
        {
        case 'I':Insert(tree,x);break;
        case 'D':Delete(tree,x);break;
        case 'F':if (Find(tree,x)) puts("True!"); else puts("False!");break;
        case 'R':printf("%d\n",Rank(tree,x));break;
        case 'T':printf("%d\n",Select(tree,x));break;
        case 'P':printf("%d\n",Pred(tree,x));break;
        case 'S':printf("%d\n",Succ(tree,x));break;
        }
    }
    fclose(stdin);
    fclose(stdout);
}

int main(int argc, char * argv[])
{
    size_balanced_tree<float> t;
    return 0;
}
