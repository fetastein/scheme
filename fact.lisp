(begin (define fact (lambda (x n) (if (> 1 x) n (fact (- x 1) (* x n))))) (fact 5 1))