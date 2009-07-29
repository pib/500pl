(define map2
  (lambda(proc l1 l2)
    (if (lor (nullp l1) 
              (nullp l2)) 
        nil
        (cons (proc (car l1) (car l2)) 
               (map2 proc (cdr l1) (cdr l2))))))

(define aldisp! (lambda (l)
                  (if (nullp l) 
                    nil
                    ((lambda()
                      (print (car (car l)))
                      (print (cdr (car  l)))
                      (aldisp! (cdr l)))))))

(define assoc
  (lambda(key alist)
    (if (nullp alist) 
         false
	(if (equal key (car(car alist)))  
             (car alist)
             (assoc key (cdr alist))))))

(define keys (cons 'eins  (cons 'zwei  (cons 'drei (cons 'vier nil)))))

(define values (cons 'one (cons 'two (cons 'three (cons 'four nil)))))

(define dictionary (map2 (lambda(m1 m2)
                           (cons m1 m2))
                         keys
                         values))

(aldisp! dictionary)


(define result (assoc 'drei dictionary))

(if (equal false result)
     (bdisp! false)
     (print (cdr result)))

(define result (assoc 'fuenf dictionary))

(if (equal false result)
     (bdisp! false)
     (print (cdr result)))
