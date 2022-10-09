import matplotlib.pyplot as plt

margin_ratio = 0.05

def create_circle(x, y, r, label):
    circle = plt.Circle((x,y), radius=r, color="lightgrey", ls="-", lw=1.0, ec="black")
    plt.annotate(label, xy=(x, y), fontsize= 38 * r, ha="center", va="center")
    return circle

def create_square_bin(bin_id, bin_size):
    bin = plt.Rectangle((bin_id*bin_size*(1+margin_ratio), 0),bin_size,bin_size,fill=False)
    return bin

def create_circular_bin(bin_id, bin_size):
    bin = plt.Circle((bin_size+bin_id*bin_size*2*(1+margin_ratio), bin_size),radius=bin_size,fill=False)
    return bin

if __name__== '__main__':
    import argparse, json
    parser = argparse.ArgumentParser(description='Circle bin packing graphics renderer')
    parser.add_argument('benchmark_solution', type=argparse.FileType('r'),
                      help='File containing the instance to render')
    parser.add_argument('is_circular_bin', type=int, default = False,
                      help='If the bin is circular (1) or square (0)')
    parser.add_argument('--topdf', type=bool, default = False,
                      help='Generate a publication ready pdf file')
    parser.add_argument('--width', type=float,
                      help='Width of the image')
    parser.add_argument('--height', type=float,
                      help='Height of the image')
    args = parser.parse_args()

    with args.benchmark_solution as f:
        s = json.load(f)
        print(s)

    def offset_circle_circular(bin_id, circle_id):
        r = s["circles"][circle_id]
        r[0] += 2*bin_id * s["binsSize"] * (1+margin_ratio)
        return r

    def offset_circle_square(bin_id, circle_id):
        r = s["circles"][circle_id]
        r[0] += bin_id * s["binsSize"] * (1+margin_ratio)
        return r

    def offset_circle(bin_id, circle_id):
        if args.is_circular_bin:
            return offset_circle_circular(bin_id, circle_id)
        else:
            return offset_circle_square(bin_id, circle_id)

    circles = [ [create_circle(*offset_circle(bin_id, circle_id)) \
        for circle_id in range(s["numberOfCircles"]) if s["bins"][circle_id]==bin_id]
            for bin_id in range(s["numberOfBins"])]

    width = height = None
    if args.width is not None:
        width = args.width
    if args.height is not None:
        height = args.height
    if width is None and height is None:
        width=7
    if width is None and height is not None:
        width=height*4.618
    elif width is not None and height is None:
        height=width/4.618

    print(width, height)

    fig= plt.gcf() #plt.subplots(figsize=(width, width/4.618)) #plt.gca()
    fig.set_size_inches(width, height)
    ax = fig.gca() #add_axes([0,0,1,1])
    ax.set_axis_off()
    ax.patch.set_visible(False)
    plt.axis("off")

    if args.topdf:
        plt.rc('font', family='serif', serif='Times')
        #plt.rc('text', usetex=True)
        plt.rc('xtick', labelsize=8)
        plt.rc('ytick', labelsize=8)
        plt.rc('axes', labelsize=8)

    for bin_id in range(s["numberOfBins"]):
        if args.is_circular_bin:
            ax.add_patch(create_circular_bin(bin_id, s["binsSize"]))
        else:
            ax.add_patch(create_square_bin(bin_id, s["binsSize"]))
        for c in circles[bin_id]:
            ax.add_patch(c)

    plt.axis('scaled')

    if args.topdf:
        #plt.figure(figsize=(width, width/2.618))
        plt.savefig("plot.pdf",dpi=1200, bbox_inches = 'tight')

    plt.show()
"""
#运行命令
python plotcircle.py output.solution  1 # circular bin
"""