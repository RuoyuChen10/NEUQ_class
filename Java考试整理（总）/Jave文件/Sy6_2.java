public class Sy6_2 {
	public static void main(String[] args) {
		// TODO �Զ����ɵķ������
    Object[] objects= {new Tiger(), new Chicken(), new Apple(), new Cat(), new Fish()};
    System.out.println("�ܳԵ��У�");
    for(int i=0;i<objects.length;i++)
    {
    	if(objects[i] instanceof Edible)
    		System.out.println(((Edible)objects[i]).howToEat());
    }
    System.out.println("\n��е��У�");
    for(int i=0;i<objects.length;i++) {	
    if(objects[i] instanceof Animal)
    		System.out.println(((Animal)objects[i]).sound());
    }
	}
}

abstract class Animal {
	public abstract String sound(); 
}

interface Edible {

    public abstract String howToEat();

}

class Tiger extends Animal{
	public String sound() {
		return "�ϻ�������";
	}
}

class Chicken extends Animal implements Edible{
	public String sound() {
		return "С��ߴߴ����ߴߴ�����Ľ�";
	}

	@Override
	public String howToEat() {
		return "С���������ųԣ��������ųԣ�Ҳ���Կ��ų�";
	}
}

class Apple implements Edible{

	@Override
	public String howToEat() {
		return "ƻ������ֱ�ӿ��ųԣ�Ҳ����ե�ɹ�֭��";
	}
	
}

class Cat extends Animal {
	public String sound(){
		return "Сè�����Ľ�";
	}
}

class Fish implements Edible{

	@Override
	public String howToEat() {
		// TODO Auto-generated method stub
		return "�������ųԣ������ųԣ����ų�";
	}
	
}