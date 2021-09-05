public class TestEdible {
    public static void main(String[] args) {
		// TODO �Զ����ɵķ������
    Object[] objects= {new Tiger(), new Chicken(), new Apple()};
    System.out.println("�ܳԵ��У�");
    for(int i=0;i<objects.length;i++)
    {
    	if(objects[i] instanceof Edible)
    		System.out.println(((Edible)objects[i]).howToEat());
    }
    System.out.println("��е��У�");
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
class Tiger extends Animal {

	@Override
	public String sound() {
		// TODO �Զ����ɵķ������
		return "�ϻ�������";
	}

}
class Chicken extends Animal implements Edible {

	@Override
	public String howToEat() {
		// TODO �Զ����ɵķ������
		return "������Կ��ų�";
	}

	@Override
	public String sound() {
		// TODO �Զ����ɵķ������
		return "С��������";
	}

}
class Apple implements Edible {

	@Override
	public String howToEat() {
		// TODO �Զ����ɵķ������
		return "ƻ������ե֭��";
	}

}
