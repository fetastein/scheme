(define iter
  (lambda (ls p l1 l2)
    (if (null? ls)
	(list l1 l2)
      (if (> (car ls) p)
	  (iter (cdr ls) p (cons (car ls) l1) l2)
	(iter (cdr ls) p l1 (cons (car ls) l2))))))
(define length_ 
  (lambda (ls n)
    (if (null? ls)
	n
      (length_ (cdr ls) (+ n 1)))))

(define length 
  (lambda (ls)
    (length_ ls 0)))

(> (length (car (cdr (iter (list  1 3 8 7 2 13) 1 nil nil)))) 1)
