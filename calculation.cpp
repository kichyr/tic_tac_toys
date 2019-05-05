#include<iostream>
#include<set>

using namespace std;

enum value{
    NIL = 0,
    CROSS = 1,
    ZERO = 2,
    DRAW = 3
};



struct TTT_tree {
    int field;
    TTT_tree *children[9];
    //кто в выйгрыше
    value state = NIL;
};

class TTT {
public:
    int count = 0;

    int start_field;
    TTT_tree root;
    set<int> possible_fields;
    //Максимальная глубина вызовов рекурсии
    int max_rec_deep;
    //флаг на подсчет только конечных полей
    int mode;

    TTT(int _max_rec_deep, int _mode): max_rec_deep(_max_rec_deep),
            mode(_mode){
    start_field = 0;
    root.field = start_field;
    build_TTT_tree(&root, CROSS, 0);
    }

    ~TTT(){
        remove_tree(&root);
    }

    //вставить на i-ю j-ю позицию
    void make_a_move(int *field, int i, int j, value v) {
        int shift = 2*(i*3+j);
        *field = *field | (v << shift);
    }
    // возвращает value элемента на i-ой j-ой позиции
    value get_v(int* field, int i, int j) {
        return static_cast<value>((*field & (3<<(2*(3*i+j))))>>(2*(3*i+j)));
    }

    void print_field(int* field) {
        for (int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++)
                cout << get_v(field, i, j);
            cout << endl;
        }
        cout << endl;
    }

    //проверить наличие тройки. возвращает value победителя
    value is_endGame(int *field) {
        for(int i = 0; i < 3; i++) {
                if(get_v(field, i, 0) != NIL && get_v(field, i, 0) == get_v(field, i, 1) && get_v(field, i, 2) == get_v(field, i, 1))
                    return get_v(field, i, 0);
                if(get_v(field, 0, i) != NIL &&  get_v(field, 0, i) == get_v(field, 1, i) && get_v(field, 0, i) == get_v(field, 2, i))
                    return get_v(field, 0, i);
        }
        
        if(get_v(field, 0, 0) != NIL &&  get_v(field, 0, 0) == get_v(field, 1, 1) && get_v(field, 2, 2) == get_v(field, 1, 1))
                    return get_v(field, 0, 0);
        if(get_v(field, 2, 0) != NIL && get_v(field, 2, 0) == get_v(field, 1, 1) && get_v(field, 0, 2) == get_v(field, 1, 1))
                    return get_v(field, 0, 2);

        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++) {
                if(get_v(field,i,j) == NIL)
                    return NIL;
        }
        return DRAW;
    }


    void build_TTT_tree(TTT_tree* t, value who, int level) {
        if((t->state = is_endGame(&(t->field))) != NIL || level >= max_rec_deep) {
            for (int i = 0; i < 3; i++) {
                for(int j = 0; j < 3; j++)
                    t->children[3*i+j] = NULL;
            return;
        }
        }

        
        for (int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++)
                if(get_v(&(t->field), i, j) == NIL) {
                    t->children[3*i+j] = new TTT_tree();
                    t->children[3*i+j]->field = t->field;
                    make_a_move(&(t->children[3*i+j]->field), i, j, who);
                    //считаем только термиальные (учитывая отражения)
                    if(mode == 1 && is_endGame(&(t->children[3*i+j]->field)))
                        count += add_all_reflections(t->children[3*i+j]->field);
                    //считаем все не учитывая отражения
                    if(mode == 0) {
                        if(possible_fields.count(t->children[3*i+j]->field) == 0) count++;
                        possible_fields.insert(t->children[3*i+j]->field);
                    }
                    //считаем все поля учитывая ориентацию
                    if(mode == 2) {
                        count += add_all_reflections(t->children[3*i+j]->field);
                    }
                    build_TTT_tree(t->children[3*i+j], static_cast<value>(who%2+1), level+1);
                }
                else {
                    t->children[3*i+j] = NULL;
                }
        
        }
    }

    void remove_tree(TTT_tree* t) {
        for(int i = 0; i < 9; i++) {
            if(t->children[i] != NULL) {
                remove_tree(t->children[i]);
                delete(t->children[i]);
            }
        }
    }

    //возвращает 0 или 1, 1 - если поле не является уже рассмотрнным или "отражением" уже рассмотренного
    int add_all_reflections(int origin_f) {
        int dop_field = origin_f;
        int field;
        int ret = 0;
        if(possible_fields.count(origin_f) == 0) {
            ret = 1;

            //вывод всех терминальных состояний с победителем ноликом
            /* if(is_endGame(&origin_f) == ZERO)
                print_field(&origin_f); */
        }
        possible_fields.insert(origin_f);
        //повороты i' = j; j' = 2 - i
        for(int k = 0; k < 3; k++) {
            field = 0;
            for(int i = 0; i < 3; i++)
                for(int j = 0; j < 3; j++) {
                    make_a_move(&field, i, j, get_v(&dop_field,2-j,i));
            }
            dop_field = field;
            possible_fields.insert(field);
            //print_field(&field);
        }

        //отражения относительно центров
        //1) i' = i, j' = 2 - j
        field = 0;
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++) {
                make_a_move(&field, i, j, get_v(&origin_f, i, 2-j));
        }
        possible_fields.insert(field);
        //print_field(&field);
        ////------------------------------------------------
        dop_field = field;
        for(int k = 0; k < 3; k++) {
            field = 0;
            for(int i = 0; i < 3; i++)
                for(int j = 0; j < 3; j++) {
                    make_a_move(&field, i, j, get_v(&dop_field,2-j,i));
            }
            dop_field = field;
            possible_fields.insert(field);
            //print_field(&field);
        }
        //--------------------------------------------------
        //2) i' = 2 - i, j' = j
        
        field = 0;
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++) {
                make_a_move(&field, i, j, get_v(&origin_f, 2 - i, j));
        }
        possible_fields.insert(field);
        //print_field(&field);

        ////------------------------------------------------
        dop_field = field;
        for(int k = 0; k < 3; k++) {
            field = 0;
            for(int i = 0; i < 3; i++)
                for(int j = 0; j < 3; j++) {
                    make_a_move(&field, i, j, get_v(&dop_field,2-j,i));
            }
            dop_field = field;
            possible_fields.insert(field);
            //print_field(&field);
        }
        //--------------------------------------------------


        // отражения отностительно диагоналей
        //
        field = 0;
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++) {
                make_a_move(&field, i, j, get_v(&origin_f, 2 - i, 2 - j));
        }
        possible_fields.insert(field);
        //print_field(&field);

        ////------------------------------------------------
        dop_field = field;
        for(int k = 0; k < 3; k++) {
            field = 0;
            for(int i = 0; i < 3; i++)
                for(int j = 0; j < 3; j++) {
                    make_a_move(&field, i, j, get_v(&dop_field,2-j,i));
            }
            dop_field = field;
            possible_fields.insert(field);
            //print_field(&field);
        }
        //--------------------------------------------------

        field = 0;
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++) {
                make_a_move(&field, i, j, get_v(&origin_f, j, i));
        }
        possible_fields.insert(field);
        //print_field(&field);
        //
        ////------------------------------------------------
        dop_field = field;
        for(int k = 0; k < 3; k++) {
            field = 0;
            for(int i = 0; i < 3; i++)
                for(int j = 0; j < 3; j++) {
                    make_a_move(&field, i, j, get_v(&dop_field,2-j,i));
            }
            dop_field = field;
            possible_fields.insert(field);
            //print_field(&field);
        }
        //--------------------------------------------------

        return ret;
    }
};


void add_all_reflections_test(TTT t) {
    //handy tests OK
    int field = 0;
    t.make_a_move(&field, 1, 2, CROSS);
    t.make_a_move(&field, 0, 0, CROSS);
    t.make_a_move(&field, 2, 1, ZERO);
    t.print_field(&field);
    t.add_all_reflections(field);
}


void test_is_endGame(TTT t) {
    int field = 0;
    t.make_a_move(&field, 0, 1, CROSS);
    t.make_a_move(&field, 1, 1, CROSS);
    t.make_a_move(&field, 2, 1, CROSS);
    t.print_field(&field);
    cout << t.is_endGame(&field);
}

int main() {
    //mode 0 - все фиксированная ориентация, mode 1 терминальные без ориентацией, mode 2 все без ориентации
    //(первый параметр - максимальная глубина рекурсии)
    // Найдем кол-во терминальных состояний без учета ориентации
    TTT ttt1(99999, 1);
    cout  << "All terminal fields, WITH the orientation(matches with wikipedia): " << ttt1.count << endl;

    TTT ttt2(99999, 0);
    cout << "all fields WITHOUT the orientation: " << ttt2.count << endl;

    TTT ttt3(99999, 2);
    cout << "all fields WITHOUT the orientation: " << ttt3.count << endl;

    return 0;
}