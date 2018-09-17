;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")

((nil . (
		 (eval . (setq cmake-ide-project-dir (projectile-project-root)))
		 (eval . (setq cmake-ide-build-dir (concat (projectile-project-root) "build")))
         )))
