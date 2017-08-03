#include "tcb.h"

TCB* head;
TCB* tail;

void insert(TCB* node) {
    if (NULL == head) {
        head = node;
        tail = node;
    } else {
        tail->next = node;
        node->prev = tail;
        tail = node;
        tail->next = head;
        head->prev = tail;
    }
}

void delete(TCB* node) {
    if (NULL != head) {
        if (head == tail) {
            head = NULL;
            tail = NULL;
        } else {
            if (head == node) {
                head = head->next;
                head->prev = tail;
                tail->next = head;
            } else if (tail == node) {
                tail = tail->prev;
                tail->next = head;
                head->prev = tail;
            } else {
                TCB* temp = head->next;
                while (temp != tail) {
                    if (temp == node) {
                        temp->next->prev = temp->prev;
                        temp->prev->next = temp->next;
                    }
                    temp = temp->next;
                }
            }
        }
    }
}