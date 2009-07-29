(define make-base-object
  (lambda()
    (lambda(msg)
      (print "message -->")
      (print  msg)
      (print " not understood by object!"))))
                   
(define make-person
  (lambda(aname)
    (define name aname)
    (define base-object (make-base-object))

    (define get-name
      (lambda()
        name))

    (define who-are-you
      (lambda()
        (print "error: method who-are-you not implemented")))

    (define self
      (lambda(msg)
        (if (equal msg 'get-name)
             name
             (if (equal msg 'who-are-you)
                  who-are-you
                  (base-object msg)))))
     self))
   
(define make-author
  (lambda(aname)
    (define books nil)
    (define super (make-person aname))

    (define new-book
      (lambda(atitle library)
        (define nbook (make-book atitle self))
        (define books (cons nbook books))
        ((library 'new-book) nbook)
        nbook))

    (define show-books
      (lambda()
        (for-each (lambda(x) ((x 'display))) books)))

    (define who-are-you
      (lambda()
        (print (super 'get-name))))

    (define self
      (lambda(msg)
        (if (equal msg 'new-book)
             new-book
             (if (equal msg 'show-books)
                  show-books
                  (if (equal msg 'who-are-you)
                       who-are-you
                       (super msg))))))
     self))
   
(define make-reader
  (lambda(aname)
    (define name aname)
    (define books nil)
    (define super (make-person aname))

    (define get-name
      (lambda()
        name))

    (define purchase-book
      (lambda(nm atitle library)
        (define nbook ((library 'sell-book) nm atitle self))
        (if (equal nbook false)
             false
             ((lambda()
                (define books (cons nbook books))
                nbook)))))

    (define borrow-book
      (lambda(nm atitle library)
        (define nbook ((library 'lend-book) nm atitle self))
        (if (equal nbook false)
             false
             ((lambda()
                (define books (cons nbook books))
                nbook)))))

    (define return-book
      (lambda(abook library)
        (if ((library 'take-back-book) abook self)
             ((lambda()
                (define books (remove abook books))
                true))
             false)))

    (define show-books
      (lambda()
        (for-each (lambda(x) ((x 'display))) books)))

    (define who-are-you
      (lambda()
        (print (super 'get-name))))

    (define self
      (lambda(msg)
        (if (equal msg 'purchase-book)
             purchase-book
             (if (equal msg 'borrow-book)
                  borrow-book
                  (if (equal msg 'return-book)
                       return-book
                       (if (equal msg 'show-books)
                            show-books
                            (if (equal msg 'who-are-you)
                                 who-are-you
                                 (super msg))))))))
     self))
   
(define make-book
  (lambda(atitle anauthor)
    (define title atitle)
    (define author anauthor)
    (define owner anauthor)
    (define holder anauthor)
    (define base-object (make-base-object))

    (define get-title
      (lambda()
        title))

    (define get-author
      (lambda()
        author))

    (define get-owner
      (lambda()
        owner))

    (define get-holder
      (lambda()
        holder))

    (define change-owner
      (lambda(new-owner)
        (define owner new-owner)))

    (define change-holder
      (lambda(new-holder)
        (define holder new-holder)))

    (define display
      (lambda()
        (print "book:")
        (print title)
        (print "by:")
        ((author 'who-are-you))
        (print "owner:")
        ((owner 'who-are-you))
        (print "holder:")
        ((holder 'who-are-you))
        (print "end-of-book")))

    (define self
      (lambda(msg)
        (if (equal msg 'get-title)
             get-title
             (if (equal msg 'get-author)
                  get-author
                  (if (equal msg 'get-owner)
                       get-owner
                       (if (equal msg 'get-holder)
                            get-holder
                            (if (equal msg 'change-owner)
                                 change-owner
                                 (if (equal msg 'change-holder)
                                      change-holder
                                      (if (equal msg 'display)
                                           display
                                           (base-object msg))))))))))
     self))
   
(define make-library
  (lambda(aname)
    (define name aname)
    (define books nil)
    (define readers nil)
    (define base-object (make-base-object))

    (define search-book-aux
      (lambda(pred anauthor atitle l)
        (if (nullp l)
             false
             (if (pred anauthor atitle (car l))
                  (car l)
                  (search-book-aux pred anauthor atitle (cdr l))))))

    (define search-book
      (lambda(anauthor atitle)
        (search-book-aux
          (lambda(author title x)
            (if (and (equal author ((x 'get-author)))
                       (equal title  ((x 'get-title))))
                 true
                 false))
          anauthor atitle books)))

    (define get-books
      (lambda()
        books))

    (define get-readers
      (lambda()
        readers))

    (define new-book
      (lambda(abook)
        ((abook 'change-owner) self)
        ((abook 'change-holder) self)
        (define books (cons abook books))
        abook))

    (define sell-book
      (lambda(author atitle reader)
        (define abook (search-book author atitle))
        (if (equal abook false)
             false
             ((lambda()
                (if (equal ((abook 'get-holder)) self)
                     ((lambda()
                        (define xreader 
                                (locate (lambda(x) 
                                          (equal reader x)) readers))
                        (if (equal xreader false)
                             (define readers (cons reader readers))
                             false)
                        ((abook 'change-owner) reader)
                        ((abook 'change-holder) reader)
                        abook))
                     false))))))

    (define lend-book
      (lambda(author atitle reader)
        (define abook (search-book author atitle))
        (if (equal abook false)
             false
             ((lambda()
                (if (equal ((abook 'get-holder)) self)
                     ((lambda()
                        (define xreader 
                                (locate (lambda(x) 
                                          (equal reader x)) readers))
                        (if (equal xreader false)
                             (define readers (cons reader readers))
                             false)
                        ((abook 'change-holder) reader)
                        abook))
                     false))))))

    (define take-back-book
      (lambda(abook reader)
        (define xreader (locate (lambda(x) (equal reader x)) readers))
        (if (equal xreader false)
             false
             ((lambda()
                (if (equal reader ((abook 'get-holder)))
                     ((lambda()
                        (define xbook 
                                (locate (lambda(x) (equal abook x)) 
                                        books))
                        (if (equal xbook false)
                             false
                             (if (equal ((abook 'get-owner)) self) 
                                  ((lambda()
                                     ((abook 'change-holder) self)
                                     true))
                                  false))))
                     false))))))

    (define show-books
      (lambda()
        (for-each (lambda(x) ((x 'display))) books)))

    (define show-readers
      (lambda()
        (for-each (lambda(x) 
                    ((x 'who-are-you))) readers)))

    (define who-are-you
      (lambda()
        (print name)))

    (define self
      (lambda(msg)
        (if (equal msg 'get-books)
             get-books
             (if (equal msg 'get-readers)
                  get-readers
                  (if (equal msg 'new-book)
                       new-book
                       (if (equal msg 'sell-book)
                            sell-book
                            (if (equal msg 'lend-book)
                                 lend-book
                                 (if (equal msg 'take-back-book)
                                      take-back-book
                                      (if (equal msg 'show-books)
                                           show-books
                                           (if (equal msg 'show-readers)
                                                show-readers
                                                (if (equal msg 'who-are-you)
                                                     who-are-you
                                                     (base-object msg))))))))))))
     self))
   
(define library
  (lambda()
    (define mylib     (make-library "net-library"))

    (define church    (make-author "Church"))
    (define kamin     (make-author "Kamin"))
    (define chazarain (make-author "Chazarain"))

    ((church 'new-book) "The Calculi of Lambda Conversion" mylib)
    ((kamin  'new-book) "Programming Languages: An Interpreter Based Approach"
                        mylib)
    ((chazarain 'new-book) "Programmer avec Scheme" mylib)

    (define charly     (make-reader "charly"))
    (define martha     (make-reader "martha"))

    (define lcbook 
            ((martha 'purchase-book) 
             church "The Calculi of Lambda Conversion" mylib))
    (define plbook 
            ((martha 'borrow-book) 
             kamin "Programming Languages: An Interpreter Based Approach" 
             mylib))
    (define chbook 
            ((charly 'borrow-book) 
             chazarain "Programmer avec Scheme" mylib))

    (print "begin of charly")
    ((charly 'show-books))
    (print "end of charly")
    (print "begin of martha")
    ((martha 'show-books))
    (print "end of martha")
    (print " ")

    (print "begin of church")
    ((church 'show-books))
    (print "end of church")
    (print " ")
    (print "begin of netlibrary")
    ((mylib 'show-books))
    (print "end of net-library")
    (print " ")
    (print "begin of readers")
    ((mylib 'show-readers))
    (print "end of readers")
    (print " ")

    ((mylib 'who-are-you))
    ((chazarain 'who-are-you))
    (print " ")

    ((lcbook 'display))
    ((plbook 'display))
    ((chbook 'display))
    (print " ")

    ((martha 'return-book) lcbook mylib)
    ((martha 'return-book) plbook mylib)
    ((charly 'return-book) chbook mylib)
    ((charly 'return-book) lcbook  mylib)
    (print " ")

    (print "begin of net-library")
    ((mylib 'show-books))
    (print "end of net-library")
    (print " ")
    (print "begin of charly")
    ((charly 'show-books))
    (print "end of charly")
    (print " ")
    (print "begin of martha")
    ((martha 'show-books))
    (print "end of martha")
    ))
