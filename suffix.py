class Node:
        def __init__(self, start, substr):
                self.start = start
                self.substr = substr
                self.branches = {}
              
def insert_into_tree(subroot, suffix, start):
        prefix_len = len(subroot.substr)
        new_suffix = str(suffix[prefix_len:])
        if(len(subroot.branches) == 0):
                left_child = Node(subroot.start, "")
                right_child = Node(start, new_suffix)
                subroot.branches[""] = left_child
                subroot.branches[new_suffix] = right_child
        else:
                for (substr, node) in subroot.branches.items():
                        if len(substr) > 0 and new_suffix.startswith(substr):
                                insert_into_tree(node, new_suffix, start)
                                break
                else:
                        new_child = Node(start, new_suffix)
                        subroot.branches[new_suffix] = new_child
              
def build_suffix_tree(t_str):
        len_str = len(t_str)
        i = len_str - 1
        root = Node(len_str, "")
        while i >= 0:
                insert_into_tree(root, str(t_str[i:]), i)
                i -= 1
        return root
              
def display_all_suffix(subroot, suffix_s_prefix, level = 0):
        if len(subroot.branches) == 0:
                print suffix_s_prefix, level
                return
        for (substr, node) in subroot.branches.items():
                display_all_suffix(node, suffix_s_prefix + substr, level + 1)
              
root = build_suffix_tree("BCABC")
display_all_suffix(root, "")
