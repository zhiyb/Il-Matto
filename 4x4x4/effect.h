#ifndef EFFECT_H
#define EFFECT_H

namespace effect {
	enum Delays {General = 750000, Breath = 1200, Scan = 80000, Expand = General, Shrink = Expand};
	enum InOut {In = true, Out = false};

	void delay(void);

	void scan(void);
	void expand(bool br);
	void shrink(bool br);
	void circle(bool in, bool br);

	void breath(bool in);
}

#endif
